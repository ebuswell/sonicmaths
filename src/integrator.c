/*
 * integrator.c
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
#include "sonicmaths/filter.h"
#include "sonicmaths/integrator.h"

/* Values for the integral of a windowed sinc function.7 samples wide */
#define WSINC_0 0.851781806f
#define WSINC_1 0.0887156468f
#define WSINC_2 -0.0167572966f
#define WSINC_3 0.00215077831f

#define LEAKINESS 0.995f

static int smaths_integrator_process(struct smaths_integrator *self) {
    float *in_buffer = smaths_parameter_get_buffer(&self->filter.in);
    if(in_buffer == NULL) {
	return -1;
    }
    float *out_buffer = gln_socket_get_buffer(&self->filter.out);
    if(out_buffer == NULL) {
	return -1;
    }

    size_t i;
    for(i = 0; i < self->filter.graph->graph.buffer_nmemb; i++) {
	float x = in_buffer[i];
	float y = self->y1 * LEAKINESS
	    + ((x + self->x6) * WSINC_3
	       + (self->x1 + self->x5) * WSINC_2
	       + (self->x2 + self->x4) * WSINC_1
	       + self->x3 * WSINC_0);

	self->x6 = self->x5;
	self->x5 = self->x4;
	self->x4 = self->x3;
	self->x3 = self->x2;
	self->x2 = self->x1;
	self->x1 = x;
	if(y == INFINITY) {
	    self->y1 = 1.0;
	} else if (y == -INFINITY) {
	    self->y1 = -1.0;
	} else {
	    self->y1 = y;
	}
	out_buffer[i] = y;
    }
    return 0;
}

int smaths_integrator_init(struct smaths_integrator *integrator, struct smaths_graph *graph) {
    int r;
    r = smaths_filter_init(&integrator->filter, graph, (gln_process_fp_t) smaths_integrator_process, integrator);
    if(r != 0) {
	return r;
    }
    integrator->y1 = 0;
    integrator->x1 = 0;
    integrator->x2 = 0;
    integrator->x3 = 0;
    integrator->x4 = 0;
    integrator->x5 = 0;
    integrator->x6 = 0;

    return 0;
}

void smaths_integrator_destroy(struct smaths_integrator *integrator) {
    smaths_filter_destroy(&integrator->filter);
}
