/** @file square.h
 *
 * Square Synth
 *
 * This produces a square wave, which corresponds to:
 *
 * @verbatim
inf sin(nwt) - sin(nw(t + skew))
 Σ  ----------------------------
n=1             n
@endverbatim
 *
 * When @c skew is 0.5, the default, this produces a true square wave.
 * Other values of @c skew will produce different ratios between upper
 * and lower values, that is, different duty cycles, with an impulse
 * train as the limit.
 *
 * If the @c scale parameter is set, the (bandlimited) amplitude of
 * the wave will be decreased such that the peak is always under 1.0.
 * This is probably not what you want unless you are using a constant
 * frequency value.  Also, this is extremely approximate; the peak
 * value should always be under 1.0, but it will not necessarily reach
 * 1.0 for all values of @c skew.
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
#ifndef SONICMATHS_SQUARE_H
#define SONICMATHS_SQUARE_H 1

#include <atomickit/atomic.h>
#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>
#include <sonicmaths/synth.h>
#include <sonicmaths/integrator.h>

/**
 * Square Synth
 *
 * See @ref struct smaths_synth
 */
struct smaths_square {
    struct smaths_synth;
    struct smaths_parameter *skew;
    atomic_bool scale;
       /**
        * Whether to scale the bandlimited waveform to 1 or not.  This
        * is probably not what you want unless the frequency is
        * constant.  For manual scaling, note that if a given
        * frequency doesn't clip, no frequency above that will clip.
        */
    struct smaths_intg_matrix *intg_matrix;
};

/**
 * Destroy square synth
 *
 * See @ref smaths_synth_destroy
 */
void smaths_square_destroy(struct smaths_square *square);

/**
 * Initialize square synth
 *
 * See @ref smaths_synth_init
 */
int smaths_square_init(struct smaths_square *square, struct smaths_graph *graph, void (*destroy)(struct smaths_square *));

struct smaths_square *smaths_square_create(struct smaths_graph *graph);

#endif /* ! SONICMATHS_SQUARE_H */
