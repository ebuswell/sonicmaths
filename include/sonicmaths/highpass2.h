/** @file highpass2.h
 *
 * Highpass filter
 *
 * @verbatim
H(s) = s^2 / (s^2 + s/Q + 1)
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
#ifndef SONICMATHS_HIGHPASS2_H
#define SONICMATHS_HIGHPASS2_H 1

#include <math.h>
#include <sonicmaths/second-order.h>

void smhighpass2(struct sm2order *filter,
		 int n, float *y, float *x, float *f, float *Q);

static inline float smhighpass2v(float y1, float y2,
				 float x, float x1, float x2,
				 float f, float Q) {
	float w, _2w, _Qw2, _4Q;
	w = f > 0.49999f ? 2 * (float) M_PI * 0.49999f
			 : 2 * (float) M_PI * f;
	_2w = 2 * w;
	_Qw2 = Q * w * w;
	_4Q = 4 * Q;
	return (  _4Q		     * (x - 2 * x1 + x2)
		- 2 * (_Qw2 - _4Q)   * y1
		- (_4Q - _2w + _Qw2) * y2)
	       / (_4Q + _2w + _Qw2);
}

#endif /* ! SONICMATHS_HIGHPASS2_H */
