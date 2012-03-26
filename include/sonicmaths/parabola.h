/** @file parabola.h
 *
 * Parabola Synth
 *
 * This produces a parabola wave, which corresponds to:
 *
 * @verbatim

inf -cos(nwt)
 Σ  ---------
n=1      2
        n
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
#ifndef SONICMATHS_PARABOLA_H
#define SONICMATHS_PARABOLA_H 1

#include <atomickit/atomic-types.h>
#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>
#include <sonicmaths/synth.h>
#include <sonicmaths/integrator.h>

/**
 * Parabola Synth
 *
 * See @ref struct smaths_synth
 */
struct smaths_bola {
    struct smaths_graph *graph; /** Graph for this synth */
    struct gln_node node; /** Node for this synth */
    struct gln_socket out; /** Output socket */
    struct smaths_parameter freq; /** Frequency divided by sample rate */
    struct smaths_parameter amp; /** Amplitude */
    struct smaths_parameter phase; /** Offset of the cycle from zero */
    struct smaths_parameter offset; /** Offset of the amplitude from zero */
    double t; /** Current time offset of the wave */
    atomic_t scale;
       /**
        * Whether to scale the bandlimited waveform to 1 or not.  This
        * is probably not what you want unless the frequency is
        * constant.  For manual scaling, note that if a given
        * frequency doesn't clip, no frequency above that will clip.
        */
    struct smaths_intg_matrix intg1_matrix;
    struct smaths_intg_matrix intg2_matrix;
};

/**
 * Destroy parabola synth
 *
 * See @ref smaths_synth_destroy
 */
static inline void smaths_bola_destroy(struct smaths_bola *bola) {
    smaths_synth_destroy((struct smaths_synth *) bola);
}

/**
 * Initialize parabola synth
 *
 * See @ref smaths_synth_init
 */
int smaths_bola_init(struct smaths_bola *bola, struct smaths_graph *graph);

#endif /* ! SONICMATHS_PARABOLA_H */
