/** @file distortion.h
 *
 * Distortion filter
 *
 * Limits input according to one of the following equations, where s
 * is @c sharpness, g is @c gain, and x is the original input.
 *
 * Lower values of @c sharpness make the sound less "warm" and vice
 * versa.
 *
 * For <tt>Exponential</tt>:
 *
 * @verbatim
         -s(gx - 1)
    log(e           + 1)
1 - --------------------
            -s
       log(e   + 1)
@endverbatim
 *
 * With a symmetrical equation for a negative x.
 *
 * For <tt>Hyperbolic</tt>:
 *
 * @verbatim
      gx
--------------
     s     1/s
(|gx|  + 1)
@endverbatim
 *
 * For <tt>Arctangent</tt>:
 *
 * @verbatim
2atan(sgx)
----------
    π
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
#ifndef SONICMATHS_DISTORTION_H
#define SONICMATHS_DISTORTION_H 1

#include <atomickit/atomic-types.h>
#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>

/**
 * Distortion filter
 *
 * See @ref struct smaths_filter
 */
struct smaths_distort {
    struct gln_node node;
    struct smaths_graph *graph;
    struct gln_socket out; /** Output */
    struct smaths_parameter in; /** Input */
    struct smaths_parameter gain;
    struct smaths_parameter sharpness;
    atomic_t kind;
};

/**
 * Destroy distortion filter
 *
 * See @ref smaths_filter_destroy
 */
void smaths_distort_destroy(struct smaths_distort *distort);

/**
 * Initialize distortion filter
 *
 * See @ref smaths_filter_init
 */
int smaths_distort_init(struct smaths_distort *distort, struct smaths_graph *graph);

#define SMATHS_EXP 1
#define SMATHS_HYP 2
#define SMATHS_ATAN 3
#define SMATHS_TUBE 4

#endif /* ! SONICMATHS_DISTORTION_H */
