/** @file integrator.h
 *
 * Implements a simple integrator.
 *
 * This takes the integral as if the sampled point output was
 * reconstructed through a windowed sinc function (Blackman seems to
 * work best) 7 samples wide---therefore there's a 3 1/2 sample delay.
 * This is then integrated and sampled with no filtering
 * (theoretically unnecessary).  The integrator is "leaky," so that DC
 * should not be introduced.
 *
 * @verbatim
y = y1 * LEAKINESS + (x + x6) * WSINC_3 + (x1 + x5) * WSINC_2
    + (x2 + x4) * WSINC_1 + x3 * WSINC_0
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
 * Integration filter
 *
 * See @ref struct smaths_filter
 */
struct smaths_integrator {
    struct smaths_filter filter;
    float y1;
    float x1;
    float x2;
    float x3;
    float x4;
    float x5;
    float x6;
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

#endif /* ! SONICMATHS_DISTORTION_H */
