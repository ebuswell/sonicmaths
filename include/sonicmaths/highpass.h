/** @file highpass.h
 *
 * Highpass filter
 *
 * @verbatim
H(s) = s^2 / (s^2 + s/Q + 1)
@endverbatim
 *
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
#ifndef SONICMATHS_HIGHPASS_H
#define SONICMATHS_HIGHPASS_H 1

#include <atomickit/atomic-float.h>
#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>
#include <sonicmaths/lowpass.h>

/**
 * Highpass filter
 *
 * See @ref struct smaths_lowpass
 */
struct smaths_highpass {
    struct smaths_graph *graph;
    struct gln_node node;
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
 * Destroy highpass filter
 *
 * See @ref smaths_lowpass_destroy
 */
static inline void smaths_highpass_destroy(struct smaths_highpass *highpass) {
    smaths_lowpass_destroy((struct smaths_lowpass *) highpass);
}

/**
 * Initialize highpass filter
 *
 * See @ref smaths_lowpass_init
 */
int smaths_highpass_init(struct smaths_highpass *highpass, struct smaths_graph *graph);

#endif /* ! SONICMATHS_HIGHPASS_H */
