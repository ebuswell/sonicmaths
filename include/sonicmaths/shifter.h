/** @file shifter.h
 *
 * Implements a frequency shifter.
 *
 */
/*
 * Copyright 2015 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 * 
 * Sonic Maths is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with Sonic Maths.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SONICMATHS_SHIFTER_H
#define SONICMATHS_SHIFTER_H 1

#include <sonicmaths/math.h>
#include <sonicmaths/oscillator.h>
#include <sonicmaths/second-order.h>
#include <sonicmaths/lowpass.h>

/**
 * Frequency Shift Filter
 */
struct smshift {
	struct smhilbert_pair coeff;
	struct sm2order filter;
	struct smosc osc;
};

/**
 * Initialize frequency shift filter
 */
int smshift_init(struct smshift *shift);

/**
 * Destroy frequency shift filter
 */
void smshift_destroy(struct smshift *shift);

/**
 * Perform a frequency shift.
 */
static inline float smshift(struct smshift *shift, float x, float f) {
	float s, c, y, y_pi_2;
	s = sinf(2 * M_PI * shift->osc.t);
	c = cosf(2 * M_PI * shift->osc.t);
	shift->osc.t += (double) f;
	shift->osc.t -= floor(shift->osc.t);
	if (f > 0.0f) {
		x = smlowpass(&shift->filter, x, 0.9995 * (0.5f - f), 8);
	} else {
		x = smlowpass(&shift->filter, x, 0.9995 * 0.5f, 8);
	}
	smhilbert_pair(&shift->coeff, x, &y, &y_pi_2);
	return y * c + y_pi_2 * s;
}

#endif /* ! SONICMATHS_SHIFTER_H */
