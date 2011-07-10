/*
 * Copyright 2011 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 2 of the License,
 * or (at your option) any later version.
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

#include <sonicmaths/clock.h>
#include <sonicmaths/parameter.h>
#include <sonicmaths/graph.h>
#include <graphline.h>

static int smaths_clock_process(struct smaths_clock *self) {
    float *clock_buffer = gln_socket_get_buffer(&self->clock);
    if(clock_buffer == NULL) {
	return -1;
    }
    float *rate_buffer = smaths_parameter_get_buffer(&self->rate);
    if(clock_buffer == NULL) {
	return -1;
    }
    size_t i;
    for(i = 0; i < self->graph->graph.buffer_nmemb; i++) {
	clock_buffer[i] = (float) self->current;
	self->current += (double) rate_buffer[i];
    }
    return 0;
}

int smaths_clock_init(struct smaths_clock *self, struct smaths_graph *graph) {
    self->current = 0.0;
    self->graph = graph;

    int r;
    r = gln_node_init(&self->node, &self->graph->graph, (gln_process_fp_t) smaths_clock_process, self);
    if(r != 0) {
	return r;
    }
    r = gln_socket_init(&self->clock, &self->node, OUTPUT);
    if(r != 0) {
	gln_node_destroy(&self->node);
	return r;
    }
    r = smaths_parameter_init(&self->rate, &self->node, 0.0f);
    if(r != 0) {
	gln_socket_destroy(&self->clock);
	gln_node_destroy(&self->node);
	return r;
    }
    return 0;
}

void smaths_clock_destroy(struct smaths_clock *self) {
    smaths_parameter_destroy(&self->rate);
    gln_socket_destroy(&self->clock);
    gln_node_destroy(&self->node);
}
