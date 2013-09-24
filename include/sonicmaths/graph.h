/** @file graph.h
 *
 * The graph for Sonic Maths.
 *
 */
/*
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
#ifndef SONICMATHS_GRAPH_H
#define SONICMATHS_GRAPH_H 1

#include <atomickit/atomic.h>
#include <atomickit/atomic-float.h>
#include <atomickit/atomic-rcp.h>
#include <graphline.h>

struct smaths_graph;

typedef int (*smaths_graph_process_fp_t)(struct smaths_graph *);

/**
 * Sonic Maths graph
 */
struct smaths_graph {
    struct gln_graph;
    struct gln_node *node;
    atomic_int frames_per_period;
    atomic_float sample_rate;
    smaths_graph_process_fp_t process;
    arcp_t run_first;
    arcp_t to_outside_sockets;
    arcp_t from_outside_sockets;
};

int smaths_graph_init(struct smaths_graph *graph, smaths_graph_process_fp_t process, void (*destroy)(struct smaths_graph *));
void smaths_graph_destroy(struct smaths_graph *graph);

int smaths_graph_process(struct smaths_graph *graph);

static inline float smaths_graph_sample_rate(struct smaths_graph *graph) {
    return atomic_float_load_explicit(&graph->sample_rate, memory_order_acquire);
}

static inline int smaths_graph_frames_per_period(struct smaths_graph *graph) {
    return atomic_load_explicit(&graph->frames_per_period, memory_order_acquire);
}

static inline int smaths_node_frames_per_period(struct gln_node *node) {
    struct smaths_graph *graph = (struct smaths_graph *) arcp_weakref_load(node->graph);
    if(graph == NULL) {
	return -1;
    }
    int ret = smaths_graph_frames_per_period(graph);
    arcp_release(graph);
    return ret;
}

static inline float smaths_graph_normalized_frequency(struct smaths_graph *graph, float f) {
    return f / smaths_graph_sample_rate(graph);
}

#define smaths_graph_normalized_rate(x, y) smaths_graph_normalized_frequency((x), (y))

static inline float smaths_graph_normalized_time(struct smaths_graph *graph, float t) {
    return t * smaths_graph_sample_rate(graph);
}

#endif /* ! SONICMATHS_GRAPH_H */
