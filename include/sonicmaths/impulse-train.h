/** @file impulse_train.h
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
 * Copyright 2013 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, version 2.
 * 
 * Sonic Maths is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * Sonic Maths.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SONICMATHS_IMPULSE_TRAIN_H
#define SONICMATHS_IMPULSE_TRAIN_H 1

#include <math.h>
void sincosf(float phi, float *sin, float *cos);
#include <sonicmaths/synth.h>

/* Magic */
#define M_A_2 8.040420309939047f

static inline float smitrain_do(float f, float t) {
	float n = floorf(1.0f / (2.0f * f)); /* the number of harmonics */
	float wt_2 = ((float) M_PI) * t; /* half angular frequency */
	float sinn1;
	float cosn1;
	float sinn;
	float cosn;
	float sinhn;
	float coshn;
	float sin1;
	float cos1;
	float sinh1;
	float cosh1;
	sincosf(wt_2, &sin1, &cos1);
	sincosf(n * wt_2, &sinn, &cosn);
	sincosf((n + 1.0f) * wt_2, &sinn1, &cosn1);
	float out = sinn * cosn1 / sin1;
	/* adjust top harmonics such that new harmonics gradually rise from
 	 * 0 */
	sinhn = sinhf(M_A_2 * n * f);
	coshn = coshf(M_A_2 * n * f);
	sinh1 = sinhf(M_A_2 * f);
	cosh1 = coshf(M_A_2 * f);
	out -= expf(M_A_2 * ((n + 1.0f) * f - 1.0f))
	       * (cosn1*sinhn*cosn*sinh1*cos1
		  + sinn1*sinhn*cosn*cosh1*sin1
		  - sinn1*coshn*sinn*cosh1*cos1
		  + cosn1*coshn*sinn*cosh1*sin1)
	       / (sinh1*sinh1 + sin1*sin1);
	return out;
}

static inline float smitrain(struct smsynth *synth, int channel,
                             float freq, float phase) {
	float ret = smitrain_do(freq, synth->t[channel] + phase);
	synth->t[channel] += freq;
	while(synth->t[channel] > 1.0f) {
		synth->t[channel] -= 1.0f;
	}
	return ret;
}

#endif /* ! SONICMATHS_IMPULSE_TRAIN */
