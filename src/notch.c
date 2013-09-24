/*
 * notch.c
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
#include <float.h>
#include <atomickit/atomic-malloc.h>
#include <graphline.h>
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/buffer.h"
#include "sonicmaths/second-order.h"
#include "sonicmaths/notch.h"

static inline float smaths_notch_stage(float x, float x1, float x2, float y1, float y2, float f, float Q) {
    float w, a, cosw2;
    w = 2 * ((float) M_PI) * f;
    a = sinf(w)/2 * Q;
    cosw2 = 2*cosf(w);

    return (
	x - cosw2 * x1 + x2
	+ cosw2 * y1 - (1 - a) * y2
	) / (1 + a);
}

static int smaths_notch_process(struct smaths_notch *notch) {
    int r, i, j;
    int nframes;
    int nchannels;
    struct smaths_buffer *in_buffer;
    float in;
    struct smaths_buffer *freq_buffer;
    float freq;
    struct smaths_buffer *Q_buffer;
    float Q;
    struct smaths_buffer *out_buffer;

    r = gln_get_buffers(3, notch->in, &in_buffer,
			notch->freq, &freq_buffer,
			notch->Q, &Q_buffer);
    if(r != 0) {
	return r;
    }

    in = smaths_parameter_go(notch->in, in_buffer);
    freq = smaths_parameter_go(notch->freq, freq_buffer);
    Q = smaths_parameter_go(notch->Q, Q_buffer);

    nframes = smaths_node_frames_per_period(notch);
    if(nframes < 0) {
	return nframes;
    }

    nchannels = SMATHS_MAX_NCHANNELS(3, in_buffer, freq_buffer, Q_buffer);

    r = smaths_2order_redim_state(notch, nchannels);
    if(r != 0) {
	return r;
    }

    out_buffer = smaths_alloc_buffer(notch->out, nframes, nchannels);
    if(out_buffer == NULL) {
	return -1;
    }

    for(i = 0; i < nframes; i++) {
	for(j = 0; j < nchannels; j++) {
	    struct smaths_2order_matrix *matrix;
	    float x, y, f;
	    matrix = &notch->matrix[j];
	    x = smaths_value(in_buffer, i, j, in);
	    f = smaths_value(freq_buffer, i, j, freq);
	    if(f > 0.4999909f) {
		f = 0.4999909f;
	    }
	    y = smaths_notch_stage(
		x, matrix->x1, matrix->x2,
		matrix->y1, matrix->y2,
		f, smaths_value(Q_buffer, i, j, Q));

	    matrix->x2 = matrix->x1;
	    matrix->x1 = x;
	    matrix->y2 = matrix->y1;
	    if(isnormal(y) || y == 0) {
		matrix->y1 = y;
	    } else if(y == INFINITY) {
		matrix->y1 = FLT_MAX;
	    } else if(y == -INFINITY) {
		matrix->y1 = -FLT_MAX;
	    } else {
		matrix->y1 = 0.0f;
	    }
	    out_buffer->data[i * nchannels + j] = y;
	}
    }
    return 0;
}

static void __smaths_notch_destroy(struct smaths_notch *notch) {
    smaths_notch_destroy(notch);
    afree(notch, sizeof(struct smaths_notch));
}

int smaths_notch_init(struct smaths_notch *notch, struct smaths_graph *graph, void (*destroy)(struct smaths_notch *)) {
    return smaths_2order_init(notch, graph, (gln_process_fp_t) smaths_notch_process, (void (*)(struct smaths_2order *)) destroy);
}

struct smaths_notch *smaths_notch_create(struct smaths_graph *graph) {
    int r;
    struct smaths_notch *ret;

    ret = amalloc(sizeof(struct smaths_notch));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_notch_init(ret, graph, __smaths_notch_destroy);
    if(r != 0) {
	afree(ret, sizeof(struct smaths_notch));
	return NULL;
    }

    return ret;
}
