/*
 * controller.c
 * 
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
#include <graphline.h>
#include "sonicmaths/graph.h"
#include "sonicmaths/controller.h"

int smaths_ctlr_init(struct smaths_ctlr *self, struct smaths_graph *graph, gln_process_fp_t func, void *arg) {
    int r;
    self->graph = graph;
    r = gln_node_init(&self->node, &graph->graph, func, arg);
    if(r != 0) {
	return r;
    }

    r = gln_socket_init(&self->ctl, &self->node, OUTPUT);
    if(r != 0) {
	gln_node_destroy(&self->node);
	return r;
    }

    r = gln_socket_init(&self->out, &self->node, OUTPUT);
    if(r != 0) {
	gln_socket_destroy(&self->ctl);
	gln_node_destroy(&self->node);
	return r;
    }

    return 0;
}

void smaths_ctlr_destroy(struct smaths_ctlr *self) {
    gln_socket_destroy(&self->out);
    gln_socket_destroy(&self->ctl);
    gln_node_destroy(&self->node);
}
