/** @file reverb.h
 *
 * Reverb filter.h
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
#ifndef SONICMATHS_REVERB_H
#define SONICMATHS_REVERB_H 1

#include <math.h>
#include <stddef.h>
#include <sonicmaths/math.h>
#include <sonicmaths/delay.h>
#include <sonicmaths/random.h>

struct smverb {
	size_t ndelays;
	struct smdelay *delays;
};

int smverb_init(struct smverb *verb, size_t delaylen, size_t ndelays);

void smverb_destroy(struct smverb *verb);

static inline float smverb(struct smverb *verb, float x, float t,
			   float tdev, float n, float g) {
	float y = 0;
	float fn;
	float fy;
	size_t i, j;

	for (fn = n, j = 0; fn > 1.0f; fn -= 1.0f, j++) {
		i = verb->delays[j].i;
		fy = smdelay_calc(&verb->delays[j],
				  fabsf(smrand_fixed_gaussian(j) * tdev + t));
		y += fy;
		verb->delays[j].x[i] = x + g * fy;
	}
	i = verb->delays[j].i;
	fy = smdelay_calc(&verb->delays[j],
			  fabsf(smrand_fixed_gaussian(j) * tdev + t));
	y += fn * fy;
	verb->delays[j].x[i] = x + g * fy;

	fy = -2 * g * y / n;
	for (j = 0; n > 0.0f; n -= 1.0f, j++) {
		i = verb->delays[j].i;
		verb->delays[j].x[i] += fy;
		verb->delays[j].i = (i + 1) % verb->delays[j].len;
	}
	return y;
}

#endif
