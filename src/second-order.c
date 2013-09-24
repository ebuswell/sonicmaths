/*
 * second-order.c
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
#include <atomickit/atomic-malloc.h>
#include <graphline.h>
#include "sonicmaths/parameter.h"
#include "sonicmaths/graph.h"
#include "sonicmaths/second-order.h"

int smaths_2order_init(struct smaths_2order *filter, struct smaths_graph *graph, gln_process_fp_t func, void (*destroy)(struct smaths_2order *)) {
    int r = -1;
    filter->matrix = amalloc(sizeof(struct smaths_2order_matrix));
    if(filter->matrix == NULL) {
	goto undo0;
    }

    r = smaths_filter_init(filter, graph, func, (void (*)(struct smaths_filter *)) destroy);
    if(r != 0) {
	goto undo1;
    }

    filter->freq = smaths_parameter_create(filter, 1.0f);
    if(filter->freq == NULL) {
	r = -1;
	goto undo2;
    }

    filter->Q = smaths_parameter_create(filter, 2.0f);
    if(filter->Q == NULL) {
	r = -1;
	goto undo3;
    }

    filter->nchannels = 1;
    memset(filter->matrix, 0, sizeof(struct smaths_2order_matrix));

    return 0;

undo3:
    arcp_release(filter->freq);
undo2:
    smaths_filter_destroy(filter);
undo1:
    afree(filter->matrix, sizeof(struct smaths_2order_matrix));
undo0:
    return r;
}

void smaths_2order_destroy(struct smaths_2order *filter) {
    arcp_release(filter->Q);
    arcp_release(filter->freq);
    smaths_filter_destroy(filter);
    afree(filter->matrix, sizeof(struct smaths_2order_matrix) * filter->nchannels);
}
