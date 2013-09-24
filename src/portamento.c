/*
 * portamento.c
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
#include <float.h>
#include <atomickit/atomic.h>
#include <atomickit/atomic-rcp.h>
#include <atomickit/atomic-malloc.h>
#include <graphline.h>
#include "sonicmaths/buffer.h"
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/filter.h"
#include "sonicmaths/portamento.h"

static int smaths_porta_redim_state(struct smaths_porta *porta, int nchannels) {
    if(porta->nchannels != nchannels) {
	int i;
	float *y1;

	y1 = arealloc(porta->y1, porta->nchannels * sizeof(float),
		      nchannels * sizeof(float));
	if(y1 == NULL) {
	    return -1;
	}

	for(i = porta->nchannels; i < nchannels; i++) {
	    y1[i] = NAN;
	}
	porta->y1 = y1;
	porta->nchannels = nchannels;
    }
    return 0;
}

static int smaths_porta_process(struct smaths_porta *porta) {
    int r, i, j;

    struct smaths_buffer *in_buffer;
    struct smaths_buffer *lag_buffer;

    r = gln_get_buffers(2, porta->in, &in_buffer,
			porta->lag, &lag_buffer);
    if(r != 0) {
	return r;
    }

    float in = smaths_parameter_go(porta->in, in_buffer);
    float lag = smaths_parameter_go(porta->lag, lag_buffer);

    int nframes = smaths_node_frames_per_period(porta);
    if(nframes < 0) {
	return nframes;
    }

    int nchannels = SMATHS_MAX_NCHANNELS(2, in_buffer, lag_buffer);

    r = smaths_porta_redim_state(porta, nchannels);
    if(r != 0) {
	return r;
    }

    struct smaths_buffer *out_buffer = smaths_alloc_buffer(porta->out, nframes, nchannels);
    if(out_buffer == NULL) {
	return -1;
    }

    for(i = 0; i < nframes; i++) {
	for(j = 0; j < nchannels; j++) {
	    float y1 = porta->y1[j];
	    float x = smaths_value(in_buffer, i, j, in);
	    float T = smaths_value(lag_buffer, i, j, lag);
	    float y = T == 0 ? x
		: y1 == x ? x
		: (y1 + ((y1 > x) ? -1 / T : 1 / T));
	    if((x < y1 && y < x)
	       || (x > y1 && y > x)) {
		y = x;
	    }
	    if(isnormal(y) || y == 0) {
		porta->y1[j] = y;
	    } else if(y == INFINITY) {
		porta->y1[j] = FLT_MAX;
	    } else if(y == -INFINITY) {
		porta->y1[j] = -FLT_MAX;
	    } else {
		porta->y1[j] = 0.0f;
	    }
	    out_buffer->data[i * nchannels + j] = y;
	}
    }
    return 0;
}

int smaths_porta_init(struct smaths_porta *porta, struct smaths_graph *graph, void (*destroy)(struct smaths_porta *)) {
    int r = -1;
    porta->y1 = amalloc(sizeof(float));
    if(porta->y1 == NULL) {
	goto undo0;
    }

    r = smaths_filter_init(porta, graph, (gln_process_fp_t) smaths_porta_process, (void (*)(struct smaths_filter *)) destroy);
    if(r != 0) {
	goto undo1;
    }

    porta->lag = smaths_parameter_create(porta, 0.0f);
    if(porta->lag == NULL) {
	r = -1;
	goto undo2;
    }

    porta->nchannels = 1;
    porta->y1[0] = NAN;

    return 0;

undo2:
    smaths_filter_destroy(porta);
undo1:
    afree(porta->y1, sizeof(float));
undo0:
    return r;
}

void smaths_porta_destroy(struct smaths_porta *porta) {
    arcp_release(porta->lag);
    smaths_filter_destroy(porta);
    afree(porta->y1, sizeof(float) * porta->nchannels);
}

static void __smaths_porta_destroy(struct smaths_porta *porta) {
    smaths_porta_destroy(porta);
    afree(porta, sizeof(struct smaths_porta));
}

struct smaths_porta *smaths_porta_create(struct smaths_graph *graph) {
    struct smaths_porta *ret;
    int r;

    ret = amalloc(sizeof(struct smaths_porta));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_porta_init(ret, graph, __smaths_porta_destroy);
    if(r != 0) {
	afree(ret, sizeof(struct smaths_porta));
	return NULL;
    }

    return ret;
}
