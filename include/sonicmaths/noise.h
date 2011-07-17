/** @file noise.h
 *
 * Noise Generator
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
#ifndef SONICMATHS_NOISE_H
#define SONICMATHS_NOISE_H 1

#include <sonicmaths/graph.h>
#include <atomickit/atomic-types.h>
#include <sonicmaths/parameter.h>

/**
 * Noise Generator
 *
 * See @ref struct smaths_synth, although this is not a strict
 * subclass since there's no @p freq input.
 */
struct smaths_noise {
    struct smaths_graph *graph;
    struct gln_node node;
    struct gln_socket out;
    struct smaths_parameter amp;
    struct smaths_parameter offset;
    atomic_t kind; /** pink, white, or red */
    float state[3]; /** state information for the pink noise generator */
};

/**
 * Destroy noise generator
 */
void smaths_noise_destroy(struct smaths_noise *noise);

/**
 * Initialize noise generator
 */
int smaths_noise_init(struct smaths_noise *noise, struct smaths_graph *graph);

#define SMATHS_WHITE 1
#define SMATHS_PINK 2
#define SMATHS_RED 3

#endif
