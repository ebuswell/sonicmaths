/*
 * sawtooth.c
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
#include <string.h>
#include <atomickit/atomic.h>
#include <graphline.h>
#include "sonicmaths/buffer.h"
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/synth.h"
#include "sonicmaths/impulse-train.h"
#include "sonicmaths/sawtooth.h"
/*#include "util.h"*/

#define SAW_SCALE_A 1.851937052f
#define SAW_SCALE_B 1.570830481f
#define SAW_SCALE_C 0.8438339747f

static inline int smaths_saw_redim_state(struct smaths_saw *saw, int nchannels) {
    if(nchannels != saw->nchannels) {
	int i;
	float *t;
	struct smaths_intg_matrix *intg_matrix;

	t = arealloc(saw->t, sizeof(float) * saw->nchannels,
		     sizeof(float) * nchannels);
	if(t == NULL) {
	    return -1;
	}

	intg_matrix = arealloc(saw->intg_matrix, sizeof(struct smaths_intg_matrix) * saw->nchannels,
			       sizeof(struct smaths_intg_matrix) * nchannels);
	if(intg_matrix == NULL) {
	    afree(saw->intg_matrix, sizeof(struct smaths_intg_matrix) * saw->nchannels);
	    afree(t, sizeof(float) * nchannels);
	    saw->nchannels = 0;
	}
	if(saw->nchannels < nchannels) {
	    for(i = saw->nchannels; i < nchannels; i++) {
		t[i] = t[i - 1];
	    }
	    memset(intg_matrix + saw->nchannels, 0, sizeof(struct smaths_intg_matrix) * (nchannels - saw->nchannels));
	}
	saw->nchannels = nchannels;
	saw->intg_matrix = intg_matrix;
	saw->t = t;
    }
    return 0;
}

static inline float smaths_saw_process_stage(float f, float t, float amp, float phase, float offset, bool scale, struct smaths_intg_matrix *intg_matrix) {
    float out = smaths_do_itrain(f, t + phase);
    out = smaths_do_integral(intg_matrix, out);

    out *= ((float) (2 * M_PI)) * f;

    if(scale) {
	out /= SAW_SCALE_A - SAW_SCALE_B
	    / (1 / (2 * f) + SAW_SCALE_C);
    }

    return out * amp + offset;
}

static int smaths_saw_process(struct smaths_saw *saw) {
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

    r = gln_get_buffers(4, saw->freq, &freq_buffer,
			saw->amp, &amp_buffer,
			saw->phase, &phase_buffer,
			saw->offset, &offset_buffer);
    if(r != 0) {
	return r;
    }


    freq = smaths_parameter_go(saw->freq, freq_buffer);
    amp = smaths_parameter_go(saw->amp, amp_buffer);
    phase = smaths_parameter_go(saw->phase, phase_buffer);
    offset = smaths_parameter_go(saw->offset, offset_buffer);

    nframes = smaths_node_frames_per_period(saw);
    if(nframes < 0) {
	return nframes;
    }

    nchannels = SMATHS_MAX_NCHANNELS(4, freq_buffer, amp_buffer, phase_buffer, offset_buffer);

    r = smaths_saw_redim_state(saw, nchannels);
    if(r != 0) {
	return r;
    }

    out_buffer = smaths_alloc_buffer(saw->out, nframes, nchannels);
    if(out_buffer == NULL) {
	return -1;
    }

    bool scale = atomic_load_explicit(&saw->scale, memory_order_consume);

    for(i = 0; i < nframes; i++) {
	for(j = 0; j < nchannels; j++) {
	    if(saw->t[j] >= 1.0f) {
		saw->t[j] -= 1.0f;
	    }
	    float f = smaths_value(freq_buffer, i, j, freq);
	    out_buffer->data[i * nchannels + j] = smaths_saw_process_stage(
		f,
		saw->t[j],
		smaths_value(amp_buffer, i, j, amp),
		smaths_value(phase_buffer, i, j, phase),
		smaths_value(offset_buffer, i, j, offset),
		scale,
		&saw->intg_matrix[j]);

	    saw->t[j] += f;
	}
    }
    return 0;
}

int smaths_saw_init(struct smaths_saw *saw, struct smaths_graph *graph, void (*destroy)(struct smaths_saw *)) {
    int r = -1;
    atomic_init(&saw->scale, false);
    saw->intg_matrix = amalloc(sizeof(struct smaths_intg_matrix));
    if(saw->intg_matrix == NULL) {
	goto undo0;
    }
    r = smaths_synth_init(saw, graph, (gln_process_fp_t) smaths_saw_process, (void (*)(struct smaths_synth *)) destroy);
    if(r != 0) {
	goto undo1;
    }
    memset(saw->intg_matrix, 0, sizeof(struct smaths_intg_matrix));
    return 0;

undo1:
    afree(saw->intg_matrix, sizeof(struct smaths_intg_matrix));
undo0:
    return -1;
}

void smaths_saw_destroy(struct smaths_saw *saw) {
    afree(saw->intg_matrix, sizeof(struct smaths_intg_matrix) * saw->nchannels);
    smaths_synth_destroy(saw);
}

static void _smaths_saw_destroy(struct smaths_saw *saw) {
    smaths_saw_destroy(saw);
    afree(saw, sizeof(struct smaths_saw));
}

struct smaths_saw *smaths_saw_create(struct smaths_graph *graph) {
    struct smaths_saw *ret;
    int r;

    ret = amalloc(sizeof(struct smaths_saw));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_saw_init(ret, graph, _smaths_saw_destroy);
    if(r != 0) {
	afree(ret, sizeof(struct smaths_saw));
    }

    return ret;
}

