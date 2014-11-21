/*
 * noise.c
 * 
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
#include <float.h>
#include <atomickit/float.h>
#include "sonicmaths/noise.h"
#include "mtrand.h"

union {
	atomic_float f;
	float v;
} gaussian_extra = { .v = NAN };

static inline float gaussian_random() {
	float s, u1, u2, a, b;
	a = ak_float_swap(&gaussian_extra.f, NAN, mo_acq_rel);
	if(!isnan(a)) {
		return a;
	}
	do {
		u1 = mt_rand_float();
		u2 = mt_rand_float();
		s = u1 * u1 + u2 * u2;
	} while(s >= 1);
	s = sqrtf(-2 * logf(s) / s);
	a = s * u1;
	b = s * u2;
	ak_float_store(&gaussian_extra.f, b, mo_release);
	return a;
}

float smnoise(enum smnoise_kind kind) {
	switch(kind) {
	case SMNOISE_GAUSSIAN:
		return gaussian_random();
	case SMNOISE_UNIFORM:
	default:
		return mt_rand_float();
	}
}
