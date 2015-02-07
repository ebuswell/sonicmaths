/** @file lag.h
 *
 * Lag Filter
 *
 * Causes instantanous changes to instead linearly progress from the old to
 * new value over a time lag.
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
#ifndef SONICMATHS_LAG_H
#define SONICMATHS_LAG_H 1

#include <stdbool.h>
#include <sonicmaths/math.h>
#include <sonicmaths/lag.h>

/**
 * Lag Filter
 */
struct smlag {
	float y1; /** The previous value */
};

/**
 * Destroy lag filter
 */
void smlag_destroy(struct smlag *lag);

/**
 * Initialize lag filter
 */
int smlag_init(struct smlag *lag);

static inline float smlag(struct smlag *lag, float x, float t, bool linear) {
	float y1 = lag->y1;
	float y;
	if (t == 0.0f) {
		y = x;
	} else if (linear) {
		y = y1 + copysignf(1 / t, x - y1);
		if ((y <= x && x <= y1)
		    || (y >= x && x >= y1)) {
			y = x;
		}
	} else {
		y = x - powf(EXP_NEG_PI, 1/t) * (x - y1);
	}
	lag->y1 = SMNORM(y);
	return y;
}

#endif
