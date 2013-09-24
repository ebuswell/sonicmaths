/*
 * impulse-train.c
 * 
 * Copyright 2013 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, version 2.
 * 
 * Sonic Maths is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Sonic Maths.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdbool.h>
#include <math.h>
#include <atomickit/atomic-rcp.h>
#include <atomickit/atomic-malloc.h>
#include <graphline.h>
#include "sonicmaths/parameter.h"
#include "sonicmaths/synth.h"
#include "sonicmaths/impulse-train.h"

static inline float smaths_itrain_process_stage(float f, float t, float amp, float phase, float offset, bool scale) {
    float out = smaths_do_itrain(f, t + phase);
    if(scale) {
	out *= 2*f;
    }
    return amp * out + offset;
}

static int smaths_itrain_process(struct smaths_itrain *itrain) {
    int r, i, j;

    struct smaths_buffer *freq_buffer;
    float freq;
    struct smaths_buffer *amp_buffer;
    float amp;
    struct smaths_buffer *phase_buffer;
    float phase;
    struct smaths_buffer *offset_buffer;
    float offset;
    struct smaths_buffer *out_buffer;
    int nframes, nchannels;

    r = gln_get_buffers(4, itrain->freq, &freq_buffer,
			itrain->amp, &amp_buffer,
			itrain->phase, &phase_buffer,
			itrain->offset, &offset_buffer);
    if(r != 0) {
	return r;
    }


    freq = smaths_parameter_go(itrain->freq, freq_buffer);
    amp = smaths_parameter_go(itrain->amp, amp_buffer);
    phase = smaths_parameter_go(itrain->phase, phase_buffer);
    offset = smaths_parameter_go(itrain->offset, offset_buffer);

    nframes = smaths_node_frames_per_period(itrain);
    if(nframes < 0) {
	return nframes;
    }

    nchannels = SMATHS_MAX_NCHANNELS(4, freq_buffer, amp_buffer, phase_buffer, offset_buffer);

    r = smaths_synth_redim_state(itrain, nchannels);
    if(r != 0) {
	return r;
    }

    out_buffer = smaths_alloc_buffer(itrain->out, nframes, nchannels);
    if(out_buffer == NULL) {
	return -1;
    }

    bool scale = atomic_load_explicit(&itrain->scale, memory_order_consume);

    for(i = 0; i < nframes; i++) {
	for(j = 0; j < nchannels; j++) {
	    if(itrain->t[j] >= 1.0f) {
		itrain->t[j] -= 1.0f;
	    }
	    float f = smaths_value(freq_buffer, i, j, freq);
	    out_buffer->data[i * nchannels + j] = smaths_itrain_process_stage(
		f,
		itrain->t[j],
		smaths_value(amp_buffer, i, j, amp),
		smaths_value(phase_buffer, i, j, phase),
		smaths_value(offset_buffer, i, j, offset),
		scale);
	    itrain->t[j] += f;
	}
    }
    return 0;
}

int smaths_itrain_init(struct smaths_itrain *itrain, struct smaths_graph *graph, void (*destroy)(struct smaths_itrain *)) {
    atomic_init(&itrain->scale, false);
    return smaths_synth_init(itrain, graph, (gln_process_fp_t) smaths_itrain_process, (void (*)(struct smaths_synth *)) destroy);
}

static void __smaths_itrain_destroy(struct smaths_itrain *itrain) {
    smaths_itrain_destroy(itrain);
    afree(itrain, sizeof(struct smaths_itrain));
}

struct smaths_itrain *smaths_itrain_create(struct smaths_graph *graph) {
    int r;
    struct smaths_itrain *ret;

    ret = amalloc(sizeof(struct smaths_itrain));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_itrain_init(ret, graph, __smaths_itrain_destroy);
    if(r != 0) {
	afree(ret, sizeof(struct smaths_itrain));
    }

    return ret;
}
