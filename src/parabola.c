/*
 * parabola.c
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
#include "sonicmaths/parabola.h"
/*#include "util.h"*/

#define BOLA_SCALE_A 1.644934067f
#define BOLA_SCALE_B 1.00000009613580760f
#define BOLA_SCALE_C 0.550546246f

static inline int smaths_bola_redim_state(struct smaths_bola *bola, int nchannels) {
    if(nchannels != bola->nchannels) {
	int i;
	float *t;
	struct smaths_intg_matrix *intg_matrix;

	t = arealloc(bola->t, sizeof(float) * bola->nchannels,
		     sizeof(float) * nchannels);
	if(t == NULL) {
	    return -1;
	}

	intg_matrix = arealloc(bola->intg_matrix, sizeof(struct smaths_intg_matrix) * bola->nchannels * 2,
			       sizeof(struct smaths_intg_matrix) * nchannels * 2);
	if(intg_matrix == NULL) {
	    afree(bola->intg_matrix, sizeof(struct smaths_intg_matrix) * bola->nchannels * 2);
	    afree(t, sizeof(float) * nchannels);
	    bola->nchannels = 0;
	}
	if(bola->nchannels < nchannels) {
	    for(i = bola->nchannels; i < nchannels; i++) {
		t[i] = t[i - 1];
	    }
	    memset(intg_matrix + bola->nchannels * 2, 0, sizeof(struct smaths_intg_matrix) * (nchannels - bola->nchannels) * 2);
	}
	bola->t = t;
	bola->intg_matrix = intg_matrix;
	bola->nchannels = nchannels;
    }
    return 0;
}

static inline float smaths_bola_process_stage(float f, float t, float amp, float phase, float offset, bool scale, struct smaths_intg_matrix *intg_matrix) {
    float out = smaths_do_itrain(f, t + phase);
    out = smaths_do_integral(&intg_matrix[0], out);
    out = smaths_do_integral(&intg_matrix[1], out);

    out *= ((float) (4 * M_PI * M_PI)) * f * f;

    if(scale) {
	out /= BOLA_SCALE_A - BOLA_SCALE_B
	    / (1 / (2 * f) + BOLA_SCALE_C);
    }

    return out * amp + offset;
}

static int smaths_bola_process(struct smaths_bola *bola) {
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

    r = gln_get_buffers(4, bola->freq, &freq_buffer,
			bola->amp, &amp_buffer,
			bola->phase, &phase_buffer,
			bola->offset, &offset_buffer);
    if(r != 0) {
	return r;
    }


    freq = smaths_parameter_go(bola->freq, freq_buffer);
    amp = smaths_parameter_go(bola->amp, amp_buffer);
    phase = smaths_parameter_go(bola->phase, phase_buffer);
    offset = smaths_parameter_go(bola->offset, offset_buffer);

    nframes = smaths_node_frames_per_period(bola);
    if(nframes < 0) {
	return nframes;
    }

    nchannels = SMATHS_MAX_NCHANNELS(4, freq_buffer, amp_buffer, phase_buffer, offset_buffer);

    r = smaths_bola_redim_state(bola, nchannels);
    if(r != 0) {
	return r;
    }

    out_buffer = smaths_alloc_buffer(bola->out, nframes, nchannels);
    if(out_buffer == NULL) {
	return -1;
    }

    bool scale = atomic_load_explicit(&bola->scale, memory_order_consume);

    for(i = 0; i < nframes; i++) {
	for(j = 0; j < nchannels; j++) {
	    if(bola->t[j] >= 1.0f) {
		bola->t[j] -= 1.0f;
	    }
	    float f = smaths_value(freq_buffer, i, j, freq);
	    out_buffer->data[i * nchannels + j] = smaths_bola_process_stage(
		f,
		bola->t[j],
		smaths_value(amp_buffer, i, j, amp),
		smaths_value(phase_buffer, i, j, phase),
		smaths_value(offset_buffer, i, j, offset),
		scale,
		&bola->intg_matrix[j*2]);

	    bola->t[j] += f;
	}
    }
    return 0;
}

int smaths_bola_init(struct smaths_bola *bola, struct smaths_graph *graph, void (*destroy)(struct smaths_bola *)) {
    int r = -1;
    atomic_init(&bola->scale, false);
    bola->intg_matrix = amalloc(sizeof(struct smaths_intg_matrix) * 2);
    if(bola->intg_matrix == NULL) {
	goto undo0;
    }
    r = smaths_synth_init(bola, graph, (gln_process_fp_t) smaths_bola_process, (void (*)(struct smaths_synth *)) destroy);
    if(r != 0) {
	goto undo1;
    }
    memset(bola->intg_matrix, 0, sizeof(struct smaths_intg_matrix) * 2);
    return 0;

undo1:
    afree(bola->intg_matrix, sizeof(struct smaths_intg_matrix) * 2);
undo0:
    return -1;
}

void smaths_bola_destroy(struct smaths_bola *bola) {
    afree(bola->intg_matrix, sizeof(struct smaths_intg_matrix) * bola->nchannels * 2);
    smaths_synth_destroy(bola);
}

static void _smaths_bola_destroy(struct smaths_bola *bola) {
    smaths_bola_destroy(bola);
    afree(bola, sizeof(struct smaths_bola));
}

struct smaths_bola *smaths_bola_create(struct smaths_graph *graph) {
    struct smaths_bola *ret;
    int r;

    ret = amalloc(sizeof(struct smaths_bola));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_bola_init(ret, graph, _smaths_bola_destroy);
    if(r != 0) {
	afree(ret, sizeof(struct smaths_bola));
    }

    return ret;
}

