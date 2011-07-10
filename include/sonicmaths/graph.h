/** @file graph.h
 *
 * The graph for Sonic Maths.
 *
 */
/*
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
#ifndef SONICMATHS_GRAPH_H
#define SONICMATHS_GRAPH_H 1

#include <graphline.h>
#include <atomickit/atomic-list.h>

/**
 * Sonic Maths graph
 */
struct smaths_graph {
    struct gln_graph graph;
    struct gln_node node;
    float sample_rate;
    atomic_list_t from_outside_sockets;
    atomic_list_t to_outside_sockets;
    atomic_list_t run_first;
};

struct smaths_graph_socketpair {
    struct gln_socket socket;
};

typedef int (*smaths_graph_process_fp_t)(void *);

struct smaths_graph_process_closure {
    smaths_graph_process_fp_t func;
    void *arg;
};

int smaths_graph_init(struct smaths_graph *graph, gln_process_fp_t process, void *arg, size_t buffer_nmemb, float sample_rate);
void smaths_graph_destroy(struct smaths_graph *graph);

int smaths_graph_process(struct smaths_graph *graph);

static inline float smaths_graph_normalized_frequency(struct smaths_graph *graph, float f) {
    return f / graph->sample_rate;
}

#define smaths_graph_normalized_rate(x, y) smaths_graph_normalized_frequency((x), (y))

static inline float smaths_graph_normalized_time(struct smaths_graph *graph, float t) {
    return t * graph->sample_rate;
}

#endif /* ! SONICMATHS_GRAPH_H */
