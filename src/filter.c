/*
 * filter.c
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

#include <atomickit/atomic-rcp.h>
#include <graphline.h>
#include "sonicmaths/parameter.h"
#include "sonicmaths/filter.h"

int smaths_filter_init(struct smaths_filter *filter, struct smaths_graph *graph, gln_process_fp_t func, void (*destroy)(struct smaths_filter *)) {
    int r;

    r = gln_node_init(filter, graph, func, (void (*)(struct gln_node *)) destroy);
    if(r != 0) {
	goto undo0;
    }

    filter->out = gln_socket_create(filter, GLNS_OUTPUT);
    if(filter->out == NULL) {
	r = -1;
	goto undo1;
    }

    filter->in = smaths_parameter_create(filter, 0.0f);
    if(filter->in == NULL) {
	r = -1;
	goto undo2;
    }

    return 0;

undo2:
    arcp_release(filter->out);
undo1:
    gln_node_destroy(filter);
undo0:
    return r;
}

void smaths_filter_destroy(struct smaths_filter *filter) {
    arcp_release(filter->in);
    arcp_release(filter->out);
    gln_node_destroy(filter);
}
