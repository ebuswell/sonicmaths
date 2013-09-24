/*
 * bandpass.c
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
#include "sonicmaths/bandpass.h"

static inline float smaths_bandpass_stage(float x, float x1 __attribute__((unused)), float x2, float y1, float y2, float f, float Q) {
    float w, a;
    w = 2 * ((float) M_PI) * f;
    a = sinf(w)/(2 * Q);

    return (a/(1 + a)) * x - (a/(1 + a)) * x2
	+ (2*cosf(w)/(1 + a)) * y1 - ((1 - a)/(1 + a)) * y2;
}

static int smaths_bandpass_process(struct smaths_bandpass *bandpass) {
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

    r = gln_get_buffers(3, bandpass->in, &in_buffer,
			bandpass->freq, &freq_buffer,
			bandpass->Q, &Q_buffer);
    if(r != 0) {
	return r;
    }

    in = smaths_parameter_go(bandpass->in, in_buffer);
    freq = smaths_parameter_go(bandpass->freq, freq_buffer);
    Q = smaths_parameter_go(bandpass->Q, Q_buffer);

    nframes = smaths_node_frames_per_period(bandpass);
    if(nframes < 0) {
	return nframes;
    }

    nchannels = SMATHS_MAX_NCHANNELS(3, in_buffer, freq_buffer, Q_buffer);

    r = smaths_2order_redim_state(bandpass, nchannels);
    if(r != 0) {
	return r;
    }

    out_buffer = smaths_alloc_buffer(bandpass->out, nframes, nchannels);
    if(out_buffer == NULL) {
	return -1;
    }

    for(i = 0; i < nframes; i++) {
	for(j = 0; j < nchannels; j++) {
	    struct smaths_2order_matrix *matrix;
	    float x, y, f;
	    matrix = &bandpass->matrix[j];
	    x = smaths_value(in_buffer, i, j, in);
	    f = smaths_value(freq_buffer, i, j, freq);
	    if(f > 0.5f) {
		f = 0.5f;
	    }
	    y = smaths_bandpass_stage(
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

static void __smaths_bandpass_destroy(struct smaths_bandpass *bandpass) {
    smaths_bandpass_destroy(bandpass);
    afree(bandpass, sizeof(struct smaths_bandpass));
}

int smaths_bandpass_init(struct smaths_bandpass *bandpass, struct smaths_graph *graph, void (*destroy)(struct smaths_bandpass *)) {
    return smaths_2order_init(bandpass, graph, (gln_process_fp_t) smaths_bandpass_process, (void (*)(struct smaths_2order *)) destroy);
}

struct smaths_bandpass *smaths_bandpass_create(struct smaths_graph *graph) {
    int r;
    struct smaths_bandpass *ret;

    ret = amalloc(sizeof(struct smaths_bandpass));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_bandpass_init(ret, graph, __smaths_bandpass_destroy);
    if(r != 0) {
	afree(ret, sizeof(struct smaths_bandpass));
	return NULL;
    }

    return ret;
}
