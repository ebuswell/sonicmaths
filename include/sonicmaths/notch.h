/** @file notch.h
 *
 * Notch filter
 *
 * @verbatim
H(s) = (s^2 + 1) / (s^2 + s/Q + 1)
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
#ifndef SONICMATHS_NOTCH_H
#define SONICMATHS_NOTCH_H 1

#include <atomickit/atomic-types.h>
#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>
#include <sonicmaths/lowpass.h>

/**
 * Notch filter
 *
 * See @ref struct smaths_lowpass
 */
struct smaths_notch {
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
 * Destroy notch filter
 *
 * See @ref smaths_lowpass_destroy
 */
inline void smaths_notch_destroy(struct smaths_notch *notch) {
    smaths_lowpass_destroy((struct smaths_lowpass *) notch);
}

/**
 * Initialize notch filter
 *
 * See @ref smaths_lowpass_init
 */
int smaths_notch_init(struct smaths_notch *notch, struct smaths_graph *graph);

#endif /* ! SONICMATHS_NOTCH_H */
