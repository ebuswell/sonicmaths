/** @file lowpass.h
 *
 * Lowpass filter
 *
 * @verbatim
H(s) = 1 / (s^2 + s/Q + 1)
@endverbatim
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
#ifndef SONICMATHS_LOWPASS_H
#define SONICMATHS_LOWPASS_H 1

#include <atomickit/atomic-types.h>
#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>

/**
 * Lowpass filter
 *
 * See @ref struct smaths_filter
 */
struct smaths_lowpass {
    struct gln_node node;
    struct smaths_graph *graph;
    struct gln_socket out; /** Output */
    struct smaths_parameter in; /** Input */
    struct smaths_parameter freq; /** The corner frequency */
    struct smaths_parameter Q; /** The filter's Q */
    atomic_float_t atten; /** Attenuation, an alternative to Q */
    float x1; /** Previous input */
    float x2; /** Previous previous input */
    float y1; /** Previous output */
    float y2; /** Previous previous output */
};

/**
 * Destroy lowpass filter
 *
 * See @ref smaths_filter_destroy
 */
void smaths_lowpass_destroy(struct smaths_lowpass *lowpass);

/**
 * Initialize lowpass filter, for subclasses
 *
 * See @ref smaths_filter_init
 */
int smaths_lowpass_subclass_init(struct smaths_lowpass *lowpass, struct smaths_graph *graph, gln_process_fp_t func, void *arg);

/**
 * Initialize lowpass filter
 *
 * See @ref smaths_filter_init
 */
int smaths_lowpass_init(struct smaths_lowpass *lowpass, struct smaths_graph *graph);

/**
 * Set filter attenuation
 *
 * You can either use atten or Q; not both.  Q = w/2a
 *
 * Ruby version: @c atten=

void cs_lowpass_set_atten(cs_lowpass_t *self, float atten);

 */

#endif /* ! SONICMATHS_LOWPASS_H */
