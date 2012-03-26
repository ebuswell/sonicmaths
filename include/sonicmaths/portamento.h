/** @file portamento.h
 *
 * Portamento Filter
 *
 * Causes instantanous changes to instead linearly progress from the old to new value over
 * a time lag.
 */
/*
 * Copyright 2012 Evan Buswell
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
#ifndef SONICMATHS_PORTAMENTO_H
#define SONICMATHS_PORTAMENTO_H 1

#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>
#include <sonicmaths/filter.h>

/**
 * Portamento Filter
 *
 * See @ref struct smaths_filter
 */
struct smaths_porta {
    struct smaths_graph *graph;
    struct gln_node node;
    struct gln_socket out; /** Output */
    struct smaths_parameter in; /** Input */
    struct smaths_parameter lag; /** The lag, in samples */
    float start; /** The value being progressed from */
    float target; /** The value being progressed to */
    float last; /** The previous value */
};

/**
 * Destroy portamento filter
 *
 * See @ref smaths_filter_destroy
 */
static inline void smaths_porta_destroy(struct smaths_porta *porta) {
    smaths_filter_destroy((struct smaths_filter *) porta);
}

/**
 * Initialize portamento filter
 *
 * See @ref smaths_filter_init
 */
int smaths_porta_init(struct smaths_porta *self, struct smaths_graph *graph);

#endif
