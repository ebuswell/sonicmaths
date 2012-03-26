/** @file bandpass.h
 *
 * Bandpass filter
 *
 * @verbatim
H(s) = (s/Q) / (s^2 + s/Q + 1)
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
#ifndef SONICMATHS_BANDPASS_H
#define SONICMATHS_BANDPASS_H 1

#include <atomickit/atomic-types.h>
#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>
#include <sonicmaths/lowpass.h>

/**
 * Bandpass filter
 *
 * See @ref struct smaths_lowpass
 */
struct smaths_bandpass {
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
 * Destroy bandpass filter
 *
 * See @ref smaths_lowpass_destroy
 */
inline void smaths_bandpass_destroy(struct smaths_bandpass *bandpass) {
    smaths_lowpass_destroy((struct smaths_lowpass *) bandpass);
}

/**
 * Initialize bandpass filter
 *
 * See @ref smaths_lowpass_init
 */
int smaths_bandpass_init(struct smaths_bandpass *bandpass, struct smaths_graph *graph);

#endif /* ! SONICMATHS_BANDPASS_H */
