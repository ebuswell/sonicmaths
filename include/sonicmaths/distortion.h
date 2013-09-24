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
 * Copyright 2013 Evan Buswell
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
#ifndef SONICMATHS_DISTORTION_H
#define SONICMATHS_DISTORTION_H 1

#include <atomickit/atomic.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>
#include <sonicmaths/filter.h>

/**
 * Distortion filter
 *
 * See @ref struct smaths_filter
 */
struct smaths_distort {
    struct smaths_filter;
    struct smaths_parameter *gain;
    struct smaths_parameter *sharpness;
    atomic_int kind;
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
int smaths_distort_init(struct smaths_distort *distort, struct smaths_graph *graph, void (*destroy)(struct smaths_distort *));

struct smaths_distort *smaths_distort_create(struct smaths_graph *graph);

enum smaths_distortion_kind {
    SMATHSD_EXP, SMATHSD_HYP, SMATHSD_ATAN
};

static inline enum smaths_distortion_kind smaths_distort_get_kind(struct smaths_distort *distort) {
    return atomic_load_explicit(&distort->kind, memory_order_consume);
}

static inline void smaths_distort_set_kind(struct smaths_distort *distort, enum smaths_distortion_kind kind) {
    atomic_store_explicit(&distort->kind, kind, memory_order_release);
}

#endif /* ! SONICMATHS_DISTORTION_H */
