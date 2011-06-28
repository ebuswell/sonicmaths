/** @file synth.h
 *
 * Structure for generic synth functions
 *
 */
/*
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
#ifndef SONICMATHS_SYNTH_H
#define SONICMATHS_SYNTH_H 1

#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>

/**
 * Structure for generic synth functions
 */
struct smaths_synth {
    struct smaths_graph *graph; /** Graph for this synth */
    struct gln_node node; /** Node for this synth */
    struct gln_socket out; /** Output socket */
    struct smaths_parameter freq; /** Frequency divided by sample rate */
    struct smaths_parameter amp; /** Amplitude */
    struct smaths_parameter phase; /** Offset of the cycle from zero */
    struct smaths_parameter offset; /** Offset of the amplitude from zero */
    double t; /** Current time offset of the wave */
};

/**
 * Destroy synth
 */
void smaths_synth_destroy(struct smaths_synth *synth);

/**
 * Initialize synth
 */
int smaths_synth_init(struct smaths_synth *synth, struct smaths_graph *graph, gln_process_fp_t func, void *arg);

#endif /* ! SONICMATHS_SYNTH_H */
