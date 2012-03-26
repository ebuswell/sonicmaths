/** @file triangle.h
 *
 * Triangle Synth
 *
 * This produces a triangle wave, which corresponds to:
 *
 * @verbatim

inf   cos(nwt) - cos(nw(t + skew))
 Σ  - ----------------------------
n=1                2
                  n
@endverbatim
 *
 * When @c skew is 0.5, the default, this produces a true triangle
 * wave.  Other values of @c skew will produce waveforms with the peak
 * off the triangle offset to one side of the wave, with a sawtooth
 * wave as the limit.
 *
 * If the @c scale parameter is set, the (bandlimited) amplitude of
 * the wave will be decreased such that the peak is always under 1.0.
 * This is probably not what you want unless you are using a constant
 * frequency value.  Also, this is extremely approximate; the peak
 * value should always be under 1.0, but it will not necessarily reach
 * 1.0 for all values of @c skew.
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
#ifndef SONICMATHS_TRIANGLE_H
#define SONICMATHS_TRIANGLE_H 1

#include <atomickit/atomic-types.h>
#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>
#include <sonicmaths/integrator.h>

/**
 * Triangle Synth
 *
 * See @ref struct smaths_synth
 */
struct smaths_triangle {
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
    struct smaths_parameter skew; /** Phase offset of the subtracted parabola. */
    struct smaths_intg_matrix intg11_matrix;
    struct smaths_intg_matrix intg12_matrix;
    struct smaths_intg_matrix intg21_matrix;
    struct smaths_intg_matrix intg22_matrix;
};

/**
 * Destroy triangle synth
 *
 * See @ref smaths_synth_destroy
 */
void smaths_triangle_destroy(struct smaths_triangle *triangle);

/**
 * Initialize triangle synth
 *
 * See @ref smaths_synth_init
 */
int smaths_triangle_init(struct smaths_triangle *triangle, struct smaths_graph *graph);

#endif /* ! SONICMATHS_TRIANGLE_H */
