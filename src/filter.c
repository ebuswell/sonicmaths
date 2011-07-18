/*
 * filter.c
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

#include <sonicmaths/filter.h>
#include <sonicmaths/parameter.h>
#include <graphline.h>

int smaths_filter_init(struct smaths_filter *self, struct smaths_graph *graph, gln_process_fp_t func, void *arg) {
    int r;
    self->graph = graph;

    r = gln_node_init(&self->node, &graph->graph, func, arg);
    if(r != 0) {
	return r;
    }

    r = gln_socket_init(&self->out, &self->node, OUTPUT);
    if(r != 0) {
	gln_node_destroy(&self->node);
	return r;
    }

    r = smaths_parameter_init(&self->in, &self->node, 0.0f);
    if(r != 0) {
	gln_socket_destroy(&self->out);
	gln_node_destroy(&self->node);
	return r;
    }

    return 0;
}

void smaths_filter_destroy(struct smaths_filter *self) {
    smaths_parameter_destroy(&self->in);
    gln_socket_destroy(&self->out);
    gln_node_destroy(&self->node);
}
