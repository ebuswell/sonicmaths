/** @file bandpass.h
 *
 * Bandpass filter
 *
 * @verbatim
H(s) = (s/Q) / (s^2 + s/Q + 1)
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
#ifndef SONICMATHS_BANDPASS_H
#define SONICMATHS_BANDPASS_H 1

#define _ISOC99_SOURCE 1
#include <math.h>
#include <sonicmaths/second-order.h>
#include <sonicmaths/math.h>

static inline float smbandpass(struct sm2order *filter, float x,
			       float f, float Q) {
	float w, a, y;
	if (f > 0.5f) {
		f = 0.5f;
	}
	w = 2 * ((float) M_PI) * f;
	a = sinf(w)/(2 * Q);

	y = (a/(1 + a)) * x
	    - (a/(1 + a)) * filter->x2
	    + (2*cosf(w)/(1 + a)) * filter->y1
	    - ((1 - a)/(1 + a)) * filter->y2;
	filter->x2 = filter->x1;
	filter->x1 = x;
	filter->y2 = filter->y1;
	filter->y1 = SMNORM(y);
	return y;
}

#endif /* ! SONICMATHS_BANDPASS_H */
