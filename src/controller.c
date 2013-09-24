/*
 * controller.c
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
#include "sonicmaths/graph.h"
#include "sonicmaths/controller.h"

int smaths_ctlr_init(struct smaths_ctlr *ctlr, struct smaths_graph *graph, gln_process_fp_t func, void (*destroy)(struct smaths_ctlr *)) {
    int r;
    r = gln_node_init(ctlr, graph, func, (void (*)(struct gln_node *)) destroy);
    if(r != 0) {
	goto undo0;
    }

    ctlr->ctl = gln_socket_create(ctlr, GLNS_OUTPUT);
    if(ctlr->ctl == NULL) {
	r = -1;
	goto undo1;
    }

    ctlr->out = gln_socket_create(ctlr, GLNS_OUTPUT);
    if(ctlr->out == NULL) {
	r = -1;
	goto undo2;
    }

    return 0;

undo2:
    arcp_release(ctlr->ctl);
undo1:
    gln_node_destroy(ctlr);
undo0:
    return r;
}

void smaths_ctlr_destroy(struct smaths_ctlr *ctlr) {
    arcp_release(ctlr->out);
    arcp_release(ctlr->ctl);
    gln_node_destroy(ctlr);
}
