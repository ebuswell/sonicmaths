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

static inline float smnormtv(float sample_rate, float t) {
	return t * sample_rate;
}

static inline float smnormfv(float sample_rate, float f) {
	return f / sample_rate;
}

#define SMFPNORM(x)		\
	(isfinite(x) ? (x)	\
	 : isnan(x) ? 0.0f	\
	 : x > 0.0f ? HUGE_VALF	\
	 : -HUGE_VALF)

static inline float smblprewarp(float w) {
	return 2.0f * atan(w / 2.0f);
}

#endif /* ! SONICMATHS_MATH_H */
