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
#include <float.h>
#include <sonicmaths/math.h>

struct smverb_tank {
	size_t i;
	size_t len;
	float f;
	float tank[];
};

struct smverb {
	size_t ntanks;
	struct smverb_tank **tanks;
};

/**
 * Destroy reverb
 */
void smverb_destroy(struct smverb *verb);

/**
 * Initialize reverb
 */
int smverb_init(struct smverb *verb,
		float echo_t, float echo_dev, size_t ntanks);

static inline float smverb(struct smverb *verb, float x, float absorbtion) {
	float ret, y, f1, f2;
	struct smverb_tank *tank;
	size_t i;

	ret = 0.0f;

	/* calculate ret, set per-tank feedback, and increment index */
	for (i = 0; i < verb->ntanks; i++) {
		tank = verb->tanks[i];
		f1 = tank->tank[tank->i];
		tank->i++;
		tank->i %= tank->len;
		f2 = tank->tank[tank->i];
		y = f1 + tank->f * (f2 - f1); /* linear interpolation */
		ret += y;
		y *= absorbtion;
		tank->tank[tank->i] = SMNORM(y); 
	}
	ret /= (float) verb->ntanks;

	/* add global feedback */
	y = -2.0f * ret * absorbtion;
	x += SMNORM(y);

	/* set input */
	for (i = 0; i < verb->ntanks; i++) {
		tank = verb->tanks[i];
		tank->tank[tank->i] += y;
	}

	return ret;
}

#endif
