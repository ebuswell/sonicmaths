/*
 * modulator.c
 * 
 * Copyright 2012 Evan Buswell
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
#include <graphline.h>
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/modulator.h"

static int smaths_modu_process(struct smaths_modu *self) {
    float *in1_buffer = smaths_parameter_get_buffer(&self->in1);
    if(in1_buffer == NULL) {
	return -1;
    }
    float *in2_buffer = smaths_parameter_get_buffer(&self->in2);
    if(in2_buffer == NULL) {
	return -1;
    }
    float *out_buffer = gln_socket_get_buffer(&self->out);
    if(out_buffer == NULL) {
	return -1;
    }
    size_t i;
    for(i = 0; i < self->graph->graph.buffer_nmemb; i++) {
	out_buffer[i] = in1_buffer[i] * in2_buffer[i];
    }
    return 0;
}

int smaths_modu_init(struct smaths_modu *self, struct smaths_graph *graph) {
    int r;
    self->graph = graph;

    r = gln_node_init(&self->node, &graph->graph, (gln_process_fp_t) smaths_modu_process, self);
    if(r != 0) {
	return r;
    }

    r = gln_socket_init(&self->out, &self->node, OUTPUT);
    if(r != 0) {
	gln_node_destroy(&self->node);
	return r;
    }

    r = smaths_parameter_init(&self->in1, &self->node, 0.0f);
    if(r != 0) {
	gln_socket_destroy(&self->out);
	gln_node_destroy(&self->node);
	return r;
    }

    r = smaths_parameter_init(&self->in2, &self->node, 0.0f);
    if(r != 0) {
	smaths_parameter_destroy(&self->in1);
	gln_socket_destroy(&self->out);
	gln_node_destroy(&self->node);
	return r;
    }

    return 0;
}

void smaths_modu_destroy(struct smaths_modu *self) {
    smaths_parameter_destroy(&self->in1);
    smaths_parameter_destroy(&self->in2);
    gln_socket_destroy(&self->out);
    gln_node_destroy(&self->node);
}
