/*
 * square.c
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
#include "sonicmaths/square.h"
/*#include "util.h"*/

#define SQUARE_SCALE_A 3.7056635625712766f
#define SQUARE_SCALE_B 1.4414686477746828f
#define SQUARE_SCALE_C 1.0790320109533682f

static inline int smaths_square_redim_state(struct smaths_square *square, int nchannels) {
    if(nchannels != square->nchannels) {
	int i;
	float *t;
	struct smaths_intg_matrix *intg_matrix;

	t = arealloc(square->t, sizeof(float) * square->nchannels,
		     sizeof(float) * nchannels);
	if(t == NULL) {
	    return -1;
	}

	intg_matrix = arealloc(square->intg_matrix, sizeof(struct smaths_intg_matrix) * square->nchannels * 2,
			       sizeof(struct smaths_intg_matrix) * nchannels * 2);
	if(intg_matrix == NULL) {
	    afree(square->intg_matrix, sizeof(struct smaths_intg_matrix) * square->nchannels * 2);
	    afree(t, sizeof(float) * nchannels);
	    square->nchannels = 0;
	}
	if(square->nchannels < nchannels) {
	    for(i = square->nchannels; i < nchannels; i++) {
		t[i] = t[i - 1];
	    }
	    memset(intg_matrix + square->nchannels * 2, 0, sizeof(struct smaths_intg_matrix) * (nchannels - square->nchannels) * 2);
	}
	square->nchannels = nchannels;
	square->intg_matrix = intg_matrix;
	square->t = t;
    }
    return 0;
}

static inline float smaths_square_process_stage(float f, float t, float amp, float phase, float offset, float skew, bool scale, struct smaths_intg_matrix *intg_matrix) {
    float out1 = smaths_do_itrain(f, t + phase);
    float out2 = smaths_do_itrain(f, t + phase + skew);
    out1 = smaths_do_integral(&intg_matrix[0], out1);
    out2 = smaths_do_integral(&intg_matrix[1], out2);

    out1 -= out2;

    out1 *= ((float) (2 * M_PI)) * f;

    if(scale) {
	/* Scales according to a hyperbola which is an
	 * approximate fit for the maximum value of the
	 * waveform along the range of skew values. */
	skew = fmodf(skew, 1);
	if(skew < 0) {
	    skew = 1 + skew;
	}
	if(skew < 0.5f) {
	    skew = 1 - skew;
	}
	float M = 1 / (2 * f);
	float oy = SQUARE_SCALE_A + SQUARE_SCALE_B / (M + SQUARE_SCALE_C);
	out1 /= oy;
	oy = ((float) M_PI) / oy;
	float oxm = 1.0f / (2.0f * (M + 1.0f));
	skew -= 1.0f;
	skew += oxm;
	M *= 2.0f;
	float dd = (2.0f * oy + M) * oxm + 1.0f;
	float sxsq = (oy + M) * skew / dd;
	out1 /= 1.0f - (M - oy * oy * oxm) * skew / dd - (oy * oxm + 1.0f) * sqrtf(sxsq * sxsq + (1.0f - M * oxm) / dd);
    }

    return out1 * amp + offset;
}

static int smaths_square_process(struct smaths_square *square) {
    int r, i, j;

    struct smaths_buffer *freq_buffer;
    float freq;
    struct smaths_buffer *amp_buffer;
    float amp;
    struct smaths_buffer *phase_buffer;
    float phase;
    struct smaths_buffer *offset_buffer;
    float offset;
    struct smaths_buffer *skew_buffer;
    float skew;
    struct smaths_buffer *out_buffer;
    int nframes, nchannels;

    r = gln_get_buffers(5, square->freq, &freq_buffer,
			square->amp, &amp_buffer,
			square->phase, &phase_buffer,
			square->offset, &offset_buffer,
			square->skew, &skew_buffer);
    if(r != 0) {
	return r;
    }

    freq = smaths_parameter_go(square->freq, freq_buffer);
    amp = smaths_parameter_go(square->amp, amp_buffer);
    phase = smaths_parameter_go(square->phase, phase_buffer);
    offset = smaths_parameter_go(square->offset, offset_buffer);
    skew = smaths_parameter_go(square->skew, skew_buffer);

    nframes = smaths_node_frames_per_period(square);
    if(nframes < 0) {
	return nframes;
    }

    nchannels = SMATHS_MAX_NCHANNELS(5, freq_buffer, amp_buffer, phase_buffer, offset_buffer, skew_buffer);

    r = smaths_square_redim_state(square, nchannels);
    if(r != 0) {
	return r;
    }

    out_buffer = smaths_alloc_buffer(square->out, nframes, nchannels);
    if(out_buffer == NULL) {
	return -1;
    }

    bool scale = atomic_load_explicit(&square->scale, memory_order_consume);

    for(i = 0; i < nframes; i++) {
	for(j = 0; j < nchannels; j++) {
	    if(square->t[j] >= 1.0f) {
		square->t[j] -= 1.0f;
	    }
	    float f = smaths_value(freq_buffer, i, j, freq);
	    out_buffer->data[i * nchannels + j] = smaths_square_process_stage(
		f,
		square->t[j],
		smaths_value(amp_buffer, i, j, amp),
		smaths_value(phase_buffer, i, j, phase),
		smaths_value(offset_buffer, i, j, offset),
		smaths_value(skew_buffer, i, j, skew),
		scale,
		&square->intg_matrix[j*2]);

	    square->t[j] += f;
	}
    }
    return 0;
}

int smaths_square_init(struct smaths_square *square, struct smaths_graph *graph, void (*destroy)(struct smaths_square *)) {
    int r = -1;

    atomic_init(&square->scale, false);

    square->intg_matrix = amalloc(sizeof(struct smaths_intg_matrix) * 2);
    if(square->intg_matrix == NULL) {
	goto undo0;
    }

    r = smaths_synth_init(square, graph, (gln_process_fp_t) smaths_square_process, (void (*)(struct smaths_synth *)) destroy);
    if(r != 0) {
	goto undo1;
    }

    square->skew = smaths_parameter_create(square, 0.5f);
    if(square->skew == NULL) {
	r = -1;
	goto undo2;
    }

    memset(square->intg_matrix, 0, sizeof(struct smaths_intg_matrix) * 2);
    return 0;

undo2:
    smaths_synth_destroy(square);
undo1:
    afree(square->intg_matrix, sizeof(struct smaths_intg_matrix) * 2);
undo0:
    return -1;
}

void smaths_square_destroy(struct smaths_square *square) {
    arcp_release(square->skew);
    smaths_synth_destroy(square);
    afree(square->intg_matrix, sizeof(struct smaths_intg_matrix) * square->nchannels * 2);
}

static void __smaths_square_destroy(struct smaths_square *square) {
    smaths_square_destroy(square);
    afree(square, sizeof(struct smaths_square));
}

struct smaths_square *smaths_square_create(struct smaths_graph *graph) {
    struct smaths_square *ret;
    int r;

    ret = amalloc(sizeof(struct smaths_square));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_square_init(ret, graph, __smaths_square_destroy);
    if(r != 0) {
	afree(ret, sizeof(struct smaths_square));
    }

    return ret;
}

