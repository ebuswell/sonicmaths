/*
 * graph.c
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
#include <graphline.h>
#include <atomickit/atomic-malloc.h>
#include <atomickit/atomic-rcp.h>
#include <atomickit/atomic-array.h>
#include "sonicmaths/graph.h"

static int __smaths_graph_node_process(struct gln_node *node) {
    int ret;
    struct smaths_graph *graph;

    graph = (struct smaths_graph *) arcp_weakref_load(node->graph);
    if(graph == NULL) {
	return -1;
    }

    ret = graph->process(graph);
    arcp_release(graph);
    return ret;
}

int smaths_graph_init(struct smaths_graph *graph, smaths_graph_process_fp_t process, void (*destroy)(struct smaths_graph *)) {
    int r = -1;

    struct aary *empty_list = aary_create(0);
    if(empty_list == NULL) {
	goto undo0;
    }
    arcp_init(&graph->run_first, empty_list);
    arcp_release(empty_list);

    empty_list = aary_create(0);
    if(empty_list == NULL) {
	goto undo1;
    }
    arcp_init(&graph->to_outside_sockets, empty_list);
    arcp_release(empty_list);

    empty_list = aary_create(0);
    if(empty_list == NULL) {
	goto undo2;
    }
    arcp_init(&graph->from_outside_sockets, empty_list);
    arcp_release(empty_list);

    r = gln_graph_init(graph, (void (*)(struct gln_graph *)) destroy);
    if(r != 0) {
	goto undo3;
    }

    graph->node = gln_node_create(graph, __smaths_graph_node_process);
    if(graph->node == NULL) {
	r = -1;
	goto undo4;
    }

    graph->process = process;

    return 0;

undo4:
    gln_graph_destroy(graph);
undo3:
    arcp_store(&graph->from_outside_sockets, NULL);
undo2:
    arcp_store(&graph->to_outside_sockets, NULL);
undo1:
    arcp_store(&graph->run_first, NULL);
undo0:
    return r;
}

void smaths_graph_destroy(struct smaths_graph *graph) {
    arcp_release(graph->node);
    arcp_store(&graph->from_outside_sockets, NULL);
    arcp_store(&graph->to_outside_sockets, NULL);
    arcp_store(&graph->run_first, NULL);
    gln_graph_destroy(graph);
}

int smaths_graph_process(struct smaths_graph *graph __attribute__((unused))) {
    /* int r = 0; */
    /* size_t i; */
    /* struct aary *run_first = (struct aary *) arcp_load(&graph->run_first); */
    /* for(i = 0; i < aary_length(run_first); i++) { */
    /* 	struct smaths_closure *closure = (struct smaths_closure *) aary_load_weak(run_first, i); */
    /* 	r |= closure->fp(closure->arg); */
    /* } */
    /* return r; */
    return 0;
}
