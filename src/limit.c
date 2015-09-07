/*
 * limit.c
 * 
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
#include <math.h>
#include "sonicmaths/limit.h"

void smlimit(enum smlimit_kind kind, int n, float *y, float *x,
	     float *sharpness) {
	float _x, _sharpness;
	switch (kind) {
	default:
	case SMLIMIT_EXP:
		while (n--) {
			_x = x[n];
			_sharpness = sharpness[n];
			y[n] = copysignf(1
					 - logf(expf(_sharpness * (1 - fabsf(_x))) + 1)
				        /* -------------------------------------------- */
					 /       logf(expf(_sharpness) + 1), _x);
		}
		return;
	case SMLIMIT_HYP:
		while (n--) {
			_x = x[n];
			_sharpness = sharpness[n];
			y[n] = _x / powf(powf(fabs(_x), _sharpness) + 1,
					 1 / _sharpness);
		}
		return;
	case SMLIMIT_ATAN:
		while (n--) {
			_x = x[n];
			_sharpness = sharpness[n];
			y[n] = 2 * atanf(_sharpness * _x)
		               / ((float) M_PI);
		}
		return;
	}
}
