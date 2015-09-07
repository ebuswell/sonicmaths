/** @file differentiator.h
 *
 * Implements a simple differentiator.
 *
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
#ifndef SONICMATHS_DIFFERENTIATOR_H
#define SONICMATHS_DIFFERENTIATOR_H 1

#include <sonicmaths/math.h>

/**
 * Integration filter
 */
struct smdiff {
	float x1;
	float x2;
	float x3;
	float x4;
	float x5;
};

/**
 * Initialize integration filter
 */
int smdiff_init(struct smdiff *diff);

/**
 * Destroy integration filter
 */
void smdiff_destroy(struct smdiff *diff);

/**
 * Integrate the signal.
 */
void smdiff(struct smdiff *diff, int n, float *y, float *x);

#endif /* ! SONICMATHS_DIFFERENTIATOR_H */
