/*
 * synth.c
 * 
 * Copyright 2010 Evan Buswell
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
#include <math.h>
#include "sonicmaths/math.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/synth.h"

int smaths_synth_init(struct smaths_synth *self, struct smaths_graph *graph, gln_process_fp_t func, void *arg) {
    self->graph = graph;
    self->t = 0.0;
    int r;
    r = gln_node_init(&self->node, &graph->graph, func, arg);
    if(r != 0) {
	return r;
    }

    r = gln_socket_init(&self->out, &self->node, OUTPUT);
    if(r != 0) {
	gln_node_destroy(&self->node);
	return r;
    }

    r = smaths_parameter_init(&self->freq, &self->node, 0.0f);
    if(r != 0) {
	gln_socket_destroy(&self->out);
	gln_node_destroy(&self->node);
	return r;
    }

    r = smaths_parameter_init(&self->phase, &self->node, frandomf());
    if(r != 0) {
	smaths_parameter_destroy(&self->freq);
	gln_socket_destroy(&self->out);
	gln_node_destroy(&self->node);
	return r;
    }

    r = smaths_parameter_init(&self->amp, &self->node, 1.0f);
    if(r != 0) {
	smaths_parameter_destroy(&self->phase);
	smaths_parameter_destroy(&self->freq);
	gln_socket_destroy(&self->out);
	gln_node_destroy(&self->node);
	return r;
    }

    r = smaths_parameter_init(&self->offset, &self->node, 0.0f);
    if(r != 0) {
	smaths_parameter_destroy(&self->amp);
	smaths_parameter_destroy(&self->phase);
	smaths_parameter_destroy(&self->freq);
	gln_socket_destroy(&self->out);
	gln_node_destroy(&self->node);
	return r;
    }

    return 0;
}

void smaths_synth_destroy(struct smaths_synth *self) {
    smaths_parameter_destroy(&self->offset);
    smaths_parameter_destroy(&self->amp);
    smaths_parameter_destroy(&self->phase);
    smaths_parameter_destroy(&self->freq);
    gln_socket_destroy(&self->out);
    gln_node_destroy(&self->node);
}
