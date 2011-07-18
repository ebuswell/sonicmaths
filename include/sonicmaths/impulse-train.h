/** @file impulse_train.h
 *
 * Impulse Train Synth
 *
 * This produces an impulse train, which corresponds to:
 *
 * @verbatim
inf
 Σ cos(n*wt)
n=1
@endverbatim
 *
 * If the @c scale parameter is set, the (bandlimited) amplitude of
 * the wave will be decreased such that the peak is always under 1.0.
 * This is probably not what you want unless you are using a constant
 * frequency value.
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
#ifndef SONICMATHS_IMPULSE_TRAIN_H
#define SONICMATHS_IMPULSE_TRAIN_H 1

#include <sonicmaths/synth.h>
#include <sonicmaths/graph.h>
#include <atomickit/atomic-types.h>

/**
 * Impulse Train Synth
 *
 * See @ref struct smaths_synth
 */
struct smaths_itrain {
    struct smaths_synth synth;
    atomic_t scale;
       /**
        * Whether to scale the bandlimited waveform to 1 or not.  This
        * is probably not what you want unless the frequency is
        * constant.  For manual scaling, note that if a given
        * frequency doesn't clip, no frequency above that will clip.
        */
};

/**
 * Destroy impulse train synth
 *
 * See @ref smaths_synth_destroy
 */
static inline void smaths_itrain_destroy(struct smaths_itrain *itrain) {
    smaths_synth_destroy(&itrain->synth);
}

/**
 * Initialize impulse train synth
 *
 * See @ref smaths_synth_init
 */
int smaths_itrain_init(struct smaths_itrain *itrain, struct smaths_graph *graph);

#endif /* SONICMATHS_IMPULSE_TRAIN_H */
