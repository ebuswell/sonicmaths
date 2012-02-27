/** @file integrator.h
 *
 * Implements a simple integrator.
 *
 * This takes the integral as if the sampled point output was
 * reconstructed through a windowed sinc function (Blackman seems to
 * work best) 9 samples wide---therefore there's a 4 1/2 sample delay.
 * This is then integrated and sampled with no filtering
 * (theoretically unnecessary).  The integrator is "leaky," so that DC
 * should not be introduced.
 *
 * @verbatim
y = y1 * LEAKINESS + (x + x8) * WSINC_4 + (x1 + x7) * WSINC_3
    + (x2 + x6) * WSINC_2 + (x3 + x5) * WSINC_1 + x4 * WSINC_0
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
#ifndef SONICMATHS_INTEGRATOR_H
#define SONICMATHS_INTEGRATOR_H 1

#include <sonicmaths/graph.h>
#include <sonicmaths/filter.h>

/**
 * Integration filter matrix
 */
struct smaths_intg_matrix {
    float y1;
    float x1;
    float x2;
    float x3;
    float x4;
    float x5;
    float x6;
    float x7;
    float x8;
};

/**
 * Integration filter
 *
 * See @ref struct smaths_filter
 */
struct smaths_integrator {
    struct smaths_filter filter;
    struct smaths_intg_matrix intg_matrix;
};

/**
 * Destroy integration filter
 *
 * See @ref smaths_filter_destroy
 */
void smaths_integrator_destroy(struct smaths_integrator *integrator);

/**
 * Initialize integration filter
 *
 * See @ref smaths_filter_init
 */
int smaths_integrator_init(struct smaths_integrator *integrator, struct smaths_graph *graph);

#define WSINC_0 0.859924743f
#define WSINC_1 0.0853056678f
#define WSINC_2 -0.0199242204f
#define WSINC_3 0.00555994799f
#define WSINC_4 -0.000883763365f

#define LEAKINESS 0.995f

static inline float smaths_do_integral(struct smaths_intg_matrix *intg_matrix, float x) {
    float y = intg_matrix->y1 * LEAKINESS
	+ ((x + intg_matrix->x8) * WSINC_4
	   + (intg_matrix->x1 + intg_matrix->x7) * WSINC_3
	   + (intg_matrix->x2 + intg_matrix->x6) * WSINC_2
	   + (intg_matrix->x3 + intg_matrix->x5) * WSINC_1
	   + intg_matrix->x4 * WSINC_0);

    intg_matrix->x8 = intg_matrix->x7;
    intg_matrix->x7 = intg_matrix->x6;
    intg_matrix->x6 = intg_matrix->x5;
    intg_matrix->x5 = intg_matrix->x4;
    intg_matrix->x4 = intg_matrix->x3;
    intg_matrix->x3 = intg_matrix->x2;
    intg_matrix->x2 = intg_matrix->x1;
    intg_matrix->x1 = x;
    if(y == INFINITY) {
	intg_matrix->y1 = 1.0f;
    } else if (y == -INFINITY) {
	intg_matrix->y1 = -1.0f;
    } else {
	intg_matrix->y1 = y;
    }
    return y;
}

#endif /* ! SONICMATHS_DISTORTION_H */
