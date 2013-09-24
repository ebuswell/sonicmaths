/*
 * clock.c
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

#include <graphline.h>
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "somicmaths/buffer.h"
#include "sonicmaths/clock.h"

static int smaths_clock_process(struct smaths_clock *clock) {
    int r, i, j;

    struct smaths_buffer *rate_buffer;

    r = gln_get_buffers(1, &clock->rate, &rate_buffer);
    if(r != 0) {
	return r;
    }

    float rate = smaths_parameter_go(&clock->rate, rate_buffer);

    int nframes = smaths_graph_frames_per_period((struct smaths_graph *) clock->graph);

    int nchannels = rate_buffer == NULL ? 1 : rate_buffer->nchannels;

    if(nchannels != clock->nchannels) {
	float *t = arealloc(clock->t, sizeof(float) * clock->nchannels,
			    sizeof(float) * nchannels);
	if(t == NULL) {
	    return -1;
	}
	if(clock->nchannels < nchannels) {
	    memset(clock->t + clock->nchannels, 0, sizeof(float) * (nchannels - clock->nchannels));
	}
	clock->t = t;
	clock->nchannels = nchannels;
    }

    struct smaths_buffer *clock_buffer = smaths_alloc_buffer(&clock->clock, nframes, nchannels);
    if(clock_buffer == NULL) {
	return -1;
    }

    for(i = 0; i < nframes; i++) {
	for(j = 0; j < nchannels; j++) {
	    clock_buffer->data[i * nchannels + j] = self->current[j];
	    self->current[j] += smaths_value(rate_buffer, i, j, rate);
	}
    }

    return 0;
}

int smaths_clock_init(struct smaths_clock *clock, struct smaths_graph *graph, void (*destroy)(struct smaths_clock *clock)) {
    int r = -1;

    clock->t = amalloc(sizeof(float));
    if(clock->t == NULL) {
	goto undo0;
    }
    clock->t[0] = 0.0f;

    r = gln_node_init(clock, graph, (gln_process_fp_t) smaths_clock_process, (void (*)(struct gln_node *)) smaths_clock_destroy);
    if(r != 0) {
	goto undo1;
    }

    clock->clock = gln_socket_create(clock, GLNS_OUTPUT);
    if(clock->clock == NULL) {
	r = -1;
	goto undo2;
    }

    clock->rate = smaths_parameter_init(clock, 0.0f);
    if(r != 0) {
	r = -1;
	goto undo3;
    }

    return 0;

undo3:
    arcp_release(clock->clock);
undo2:
    gln_node_destroy(clock);
undo1:
    afree(clock->t, sizeof(float));
undo0:
    return r;
}

void smaths_clock_destroy(struct smaths_clock *clock) {
    arcp_release(clock->clock);
    arcp_release(clock->rate);
    afree(clock->t, sizeof(float) * clock->nchannels);
    gln_node_destroy(&clock->node);
}

static void __smaths_clock_destroy(struct smaths_clock *clock) {
    smaths_clock_destroy(clock);
    afree(clock, sizeof(struct smaths_clock));
}

struct smaths_clock *smaths_clock_create(struct smaths_graph *graph) {
    int r;

    struct smaths_clock *clock = amalloc(sizeof(struct smaths_clock));
    if(clock == NULL) {
	return NULL;
    }

    r = smaths_clock_init(clock, graph, __smaths_clock_destroy);
    if(r != 0) {
	afree(clock, sizeof(struct smaths_clock));
	return NULL;
    }

    return clock;
}
