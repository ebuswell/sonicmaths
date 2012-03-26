/** @file sine.h
 *
 * Sine Wave Synth
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
#ifndef SONICMATHS_SINE_H
#define SONICMATHS_SINE_H 1

#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>
#include <sonicmaths/synth.h>

/**
 * Sine Wave Synth
 *
 * See @ref struct smaths_synth
 */
struct smaths_sine {
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
 * Destroy sine synth
 *
 * See @ref smaths_synth_destroy
 */
static inline void smaths_sine_destroy(struct smaths_sine *sine) {
    smaths_synth_destroy((struct smaths_synth *) sine);
}

/**
 * Initialize sine synth
 *
 * See @ref smaths_synth_destroy
 */
int smaths_sine_init(struct smaths_sine *sine, struct smaths_graph *graph);

#endif /* ! SONICMATHS_SINE_H */
