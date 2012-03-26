/** @file filter.h
 *
 * Generic Filter
 *
 * In Sonic Maths, a filter is just something which performs a
 * transformation on the input.  It is not necessary for it to involve
 * a frequency parameter, but it often does.
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
#ifndef SONICMATHS_FILTER_H
#define SONICMATHS_FILTER_H 1

#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>

/**
 * Generic Filter
 */
struct smaths_filter {
    struct smaths_graph *graph;
    struct gln_node node;
    struct gln_socket out; /** Output */
    struct smaths_parameter in; /** Input */
};

/**
 * Destroy Filter
 */
void smaths_filter_destroy(struct smaths_filter *filter);

/**
 * Initialize Filter
 */
int smaths_filter_init(struct smaths_filter *filter, struct smaths_graph *graph, gln_process_fp_t func, void *arg);

#endif /* ! SONICMATHS_FILTER_H */
