/** @file math.h
 *
 * Miscellaneous mathematical functions for Sonic Maths.
 *
 */
/*
 * Copyright 2015 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, version 2.
 * 
 * Sonic Maths is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with Sonic Maths.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SONICMATHS_MATH_H
#define SONICMATHS_MATH_H 1

#include <stdlib.h>
#include <math.h>
#include <float.h>

static inline float smaths_normtime(float sample_rate, float t) {
	return t * sample_rate;
}

static inline float smaths_normfreq(float sample_rate, float f) {
	return f / sample_rate;
}

#define SMNORM(x) ((isnormal(x) || x == 0.0f) ? x	\
		   : x == INFINITY ? FLT_MAX		\
		   : x == -INFINITY ? -FLT_MAX		\
		   : 0.0f)

static inline float smaths_wsinc(float x, float n) {
	float w1;
	float w2;
	w1 = (float) M_PI * (x - n / 2.0f);
	if (w1 == 0.0f) {
		return 1.0f;
	}
	w2 = 2.0f * w1 / n;
	return (0.42f + 0.5f * cosf(w2) + 0.08f * cosf(2.0f * w2))
		* sinf(w1) / w1;
}

#endif /* ! SONICMATHS_MATH_H */
