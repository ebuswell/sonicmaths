/*
 * portamento.c
 * 
 * Copyright 2011 Evan Buswell
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
#include <atomickit/atomic-float.h>
#include <graphline.h>
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/filter.h"
#include "sonicmaths/portamento.h"

static int smaths_porta_process(struct smaths_porta *self) {
    float *in_buffer = smaths_parameter_get_buffer(&self->in);
    if(in_buffer == NULL) {
	return -1;
    }
    float *lag_buffer = smaths_parameter_get_buffer(&self->lag);
    if(lag_buffer == NULL) {
	return -1;
    }
    float *out_buffer = gln_socket_get_buffer(&self->out);
    if(out_buffer == NULL) {
	return -1;
    }
    size_t i;
    for(i = 0; i < self->graph->graph.buffer_nmemb; i++) {
	if(in_buffer[i] != self->target) {
	    self->target = in_buffer[i];
	    if(self->last == INFINITY) {
		self->last = FLT_MAX;
	    } else if(self->last == -INFINITY) {
		self->last = FLT_MIN;
	    }
	    self->start = self->last;
	}
	if(self->last != self->target) {
	    self->last += (self->target - self->start) / lag_buffer[i];
	    if(self->start <= self->target) {
		/* going up */
		if(self->last > self->target) {
		    self->last = self->target;
		}
	    } else {
		/* going down */
		if(self->last < self->target) {
		    self->last = self->target;
		}
	    }
	}
	out_buffer[i] = self->last;
    }
    return 0;
}

int smaths_porta_init(struct smaths_porta *self, struct smaths_graph *graph) {
    int r;
    r = smaths_filter_init((struct smaths_filter *) self, graph, (gln_process_fp_t) smaths_porta_process, self);
    if(r != 0) {
	return r;
    }

    r = smaths_parameter_init(&self->lag, &self->node, 0.0f);
    if(r != 0) {
	smaths_filter_destroy((struct smaths_filter *) self);
	return r;
    }

    self->start = 0.0f;
    self->target = 0.0f;
    self->last = 0.0f;

    return 0;
}
