/*
 * modulator.c
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
#include "sonicmaths/modulator.h"

static int smaths_modu_process(struct smaths_modu *modu) {
    int r, i, j;

    struct smaths_buffer *in1_buffer;
    struct smaths_buffer *in2_buffer;

    r = gln_get_buffers(2, modu->in1, &in1_buffer,
			modu->in2, &in2_buffer);
    if(r != 0) {
	return r;
    }

    float in1 = smaths_parameter_go(modu->in1, in1_buffer);
    float in2 = smaths_parameter_go(modu->in2, in2_buffer);

    int nframes = smaths_node_frames_per_period(modu);
    if(nframes < 0) {
	return nframes;
    }

    int nchannels = SMATHS_MAX_NCHANNELS(2, in1_buffer, in2_buffer);

    struct smaths_buffer *out_buffer = smaths_alloc_buffer(modu->out, nframes, nchannels);
    if(out_buffer == NULL) {
	return -1;
    }

    for(i = 0; i < nframes; i++) {
	for(j = 0; j < nchannels; j++) {
	    out_buffer->data[i * nchannels + j] = smaths_value(in1_buffer, i, j, in1) * smaths_value(in2_buffer, i, j, in2);
	}
    }
    return 0;
}

int smaths_modu_init(struct smaths_modu *modu, struct smaths_graph *graph, void (*destroy)(struct smaths_modu *)) {
    int r;

    r = gln_node_init(modu, graph, (gln_process_fp_t) smaths_modu_process, (void (*)(struct gln_node *)) destroy);
    if(r != 0) {
	goto undo0;
    }

    modu->out = gln_socket_create(modu, GLNS_OUTPUT);
    if(modu->out == NULL) {
	r = -1;
	goto undo1;
    }

    modu->in1 = smaths_parameter_create(modu, 1.0f);
    if(modu->in1 == NULL) {
	r = -1;
	goto undo2;
    }

    modu->in2 = smaths_parameter_create(modu, 1.0f);
    if(modu->in2 == NULL) {
	r = -1;
	goto undo3;
    }

    return 0;

undo3:
    arcp_release(modu->in1);
undo2:
    arcp_release(modu->out);
undo1:
    gln_node_destroy(modu);
undo0:
    return r;
}

void smaths_modu_destroy(struct smaths_modu *modu) {
    arcp_release(modu->in2);
    arcp_release(modu->in1);
    arcp_release(modu->out);
    gln_node_destroy(modu);
}

static void __smaths_modu_destroy(struct smaths_modu *modu) {
    smaths_modu_destroy(modu);
    afree(modu, sizeof(struct smaths_modu));
}

struct smaths_modu *smaths_modu_create(struct smaths_graph *graph) {
    struct smaths_modu *ret;
    int r;

    ret = amalloc(sizeof(struct smaths_modu));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_modu_init(ret, graph, __smaths_modu_destroy);
    if(r != 0) {
	afree(ret, sizeof(struct smaths_modu));
	return NULL;
    }

    return ret;
}
