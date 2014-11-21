/** @file highpass.h
 *
 * Highpass filter
 *
 * @verbatim
H(s) = s^2 / (s^2 + s/Q + 1)
@endverbatim
 *
 */
/*
 * Copyright 2013 Evan Buswell
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
#ifndef SONICMATHS_HIGHPASS_H
#define SONICMATHS_HIGHPASS_H 1

#include <sonicmaths/second-order.h>

static inline float smhighpass_do(struct sm2order_matrix *matrix, float x,
                                  float f, float Q) {
	float w, a, cosw, y;
	if(f > 0.5f) {
		f = 0.5f;
	}
	w = 2 * ((float) M_PI) * f;
	a = sinf(w)/(2 * Q);
	cosw = cosf(w);

	y = ((1 + cosw)/(1 + a)) * ((x + matrix->x2)/2 - matrix->x1)
	    + (2*cosw/(1 + a)) * matrix->y1 - ((1 - a)/(1 + a)) * matrix->y2;
	matrix->x2 = matrix->x1;
	matrix->x1 = x;
	matrix->y2 = matrix->y1;
	if(isnormal(y) || y == 0.0f) {
		matrix->y1 = y;
	} else if(y == INFINITY) {
		matrix->y1 = FLT_MAX;
	} else if(y == -INFINITY) {
		matrix->y1 = -FLT_MAX;
	} else {
		matrix->y1 = 0.0f;
	}
	return y;
}

static inline float smhighpass(struct sm2order *filter, int channel,
                               float x, float f, float Q) {
	return smhighpass_do(&filter->matrix[channel], x, f, Q);
}


#endif /* ! SONICMATHS_HIGHPASS_H */
