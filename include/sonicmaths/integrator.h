/** @file integrator.h
 *
 * Implements a simple integrator.
 *
 * This takes the integral as if the sampled point output was reconstructed
 * through a windowed sinc function (Blackman seems to work best) 9 samples
 * wide---therefore there's a 4 1/2 sample delay. This is then integrated and
 * sampled with no filtering (theoretically unnecessary).  The integrator is
 * "leaky," so that DC should not be introduced.
 *
 * @verbatim
y = y1 * LEAKINESS + (x + x8) * WSINC_4 + (x1 + x7) * WSINC_3
    + (x2 + x6) * WSINC_2 + (x3 + x5) * WSINC_1 + x4 * WSINC_0
@endverbatim
 *
 */
/*
 * Copyright 2015 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 * 
 * Sonic Maths is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with Sonic Maths.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SONICMATHS_INTEGRATOR_H
#define SONICMATHS_INTEGRATOR_H 1

#include <sonicmaths/math.h>

/**
 * Integration filter
 */
struct smintg {
	float y1;
	float x1;
	float x2;
	float x3;
	float x4;
	float x5;
	float x6;
};

/**
 * Initialize integration filter
 */
int smintg_init(struct smintg *intg);

/**
 * Destroy integration filter
 */
void smintg_destroy(struct smintg *intg);

/**
 * Integrate the signal.
 */
void smintg(struct smintg *intg, int n, float *y, float *x);

#endif /* ! SONICMATHS_INTEGRATOR_H */
