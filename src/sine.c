/*
 * sine.c
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
#include <math.h>
#include <atomickit/atomic-rcp.h>
#include <atomickit/atomic-malloc.h>
#include <graphline.h>
#include "sonicmaths/parameter.h"
#include "sonicmaths/synth.h"
#include "sonicmaths/sine.h"

static inline float smaths_sine_process_stage(float t, float amp, float phase, float offset) {
    return amp * sinf(2 * ((float) M_PI) * (t + phase)) + offset;
}

static int smaths_sine_process(struct smaths_synth *sine) {
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

    r = gln_get_buffers(4, sine->freq, &freq_buffer,
			sine->amp, &amp_buffer,
			sine->phase, &phase_buffer,
			sine->offset, &offset_buffer);
    if(r != 0) {
	return r;
    }


    freq = smaths_parameter_go(sine->freq, freq_buffer);
    amp = smaths_parameter_go(sine->amp, amp_buffer);
    phase = smaths_parameter_go(sine->phase, phase_buffer);
    offset = smaths_parameter_go(sine->offset, offset_buffer);

    nframes = smaths_node_frames_per_period(sine);
    if(nframes < 0) {
	return nframes;
    }

    nchannels = SMATHS_MAX_NCHANNELS(4, freq_buffer, amp_buffer, phase_buffer, offset_buffer);

    r = smaths_synth_redim_state(sine, nchannels);
    if(r != 0) {
	return r;
    }

    out_buffer = smaths_alloc_buffer(sine->out, nframes, nchannels);
    if(out_buffer == NULL) {
	return -1;
    }

    for(i = 0; i < nframes; i++) {
	for(j = 0; j < nchannels; j++) {
	    if(sine->t[j] >= 1.0f) {
		sine->t[j] -= 1.0f;
	    }
	    out_buffer->data[i * nchannels + j] = smaths_sine_process_stage(
		sine->t[j],
		smaths_value(amp_buffer, i, j, amp),
		smaths_value(phase_buffer, i, j, phase),
		smaths_value(offset_buffer, i, j, offset));
	    sine->t[j] += smaths_value(freq_buffer, i, j, freq);
	    if(!isnormal(sine->t[j])) {
		sine->t[j] = 0;
	    }
	}
    }
    return 0;
}

int smaths_sine_init(struct smaths_sine *sine, struct smaths_graph *graph, void (*destroy)(struct smaths_sine *)) {
    return smaths_synth_init(sine, graph, (gln_process_fp_t) smaths_sine_process, (void (*)(struct smaths_synth *)) destroy);
}

static void __smaths_sine_destroy(struct smaths_sine *sine) {
    smaths_sine_destroy(sine);
    afree(sine, sizeof(struct smaths_sine));
}

struct smaths_sine *smaths_sine_create(struct smaths_graph *graph) {
    int r;
    struct smaths_sine *ret;

    ret = amalloc(sizeof(struct smaths_sine));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_sine_init(ret, graph, __smaths_sine_destroy);
    if(r != 0) {
	afree(ret, sizeof(struct smaths_sine));
    }

    return ret;
}
