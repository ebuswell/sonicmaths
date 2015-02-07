/** @file impulse-train.h
 *
 * Impulse Train Synth
 *
 * This produces an impulse train, which corresponds to:
 *
 * @verbatim
inf
 Σ cos(nwt)
n=1
@endverbatim
 *
 * If the @c scale parameter is set, the (bandlimited) amplitude of the wave
 * will be decreased such that the peak is always under 1.0. This is probably
 * not what you want unless you are using a constant frequency value.
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
#ifndef SONICMATHS_IMPULSE_TRAIN_H
#define SONICMATHS_IMPULSE_TRAIN_H 1

#include <math.h>
#include <sonicmaths/oscillator.h>

/* Magic */
#define SMTRAIN_A 16.080840619878092
#define EXP_NEG_A_2 0.0003221735089294733

static inline float smitrain_do(float f, double t) {
	float n = floorf(1.0f / (2.0f * f)); /* the number of harmonics */
	float n1 = n + 1.0f;
	float wt_2 = (float) (M_PI * t); /* half angular frequency */
	float wt = 2 * wt_2;
	float coswt;
	float af;
	float out;

	out = cosf(n1 * wt_2) * sinf(n * wt_2) / sinf(wt_2);
	/* adjust top harmonics such that new harmonics gradually rise from
 	 * 0 */
	coswt = cosf(wt);
	af = SMTRAIN_A * f;
	out -= EXP_NEG_A_2
	       * (coswt
		  - expf(n * af) * cosf(n1 * wt)
		  + expf(n1 * af) * cosf(n * wt)
		  + expf(af))
	       / (2.0f * (coshf(af) - coswt));
	return out;
}

static inline float smitrain(struct smosc *osc, float freq, float phase) {
	float ret = smitrain_do(freq, osc->t + (double) phase);
	osc->t += (double) freq;
	osc->t -= floor(osc->t);
	return ret;
}

#endif /* ! SONICMATHS_IMPULSE_TRAIN */
