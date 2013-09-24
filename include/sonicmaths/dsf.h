/** @file dsf.h
 *
 * Discrete Summation Formula Synth
 *
 * This produces a wave corresponding to the equation
 *
 * @verbatim
inf  n-1
 Σ  b    * sin(nwt)
n=1
@endverbatim
 *
 * Where b is "brightness", valued from 0 (a sine wave, @ref sine.h) to 1 (a
 * cotangent/2 wave, containing equal amounts of all frequencies, @ref cot.h).
 *
 * If the @c scale parameter is set, the amplitude of the wave will be decreased such
 * that the peak is always under 1.0.
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
#ifndef SONICMATHS_DSF_H
#define SONICMATHS_DSF_H 1

#include <atomickit/atomic-types.h>
#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>

/**
 * Discrete Summation Formula Synth
 *
 * See @ref struct smaths_synth
 */
struct smaths_dsf {
    struct smaths_synth;
    struct smaths_parameter bright;
       /** Brightness, from 0-1, see discussion at @ref dsf.h */
    atomic_bool scale;
       /**
        * Whether to scale the bandlimited waveform to 1 or not.  This
        * is probably not what you want unless the frequency is
        * constant.  For manual scaling, note that if a given
        * frequency doesn't clip, no frequency above that will clip.
        */
};

/**
 * Destroy discrete summation formula synth
 *
 * See @ref smaths_synth_destroy
 */
void smaths_dsf_destroy(struct smaths_dsf *dsf);

/**
 * Initialize discrete summation formula synth
 *
 * See @ref smaths_synth_init
 */
int smaths_dsf_init(struct smaths_dsf *dsf, struct smaths_graph *graph);

#endif /* SONICMATHS_DSF_H */
