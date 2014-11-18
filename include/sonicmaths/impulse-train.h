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
#include <sonicmaths/synth.h>

/* Magic */
#define M_MNTS 9634303.96f
#define M_MNTS_NSQR 0.000322173509f

static inline float smitrain_do(float f, float t) {
	float n = floorf(1.0f / (2.0f * f)); /* the number of harmonics */
	float wt_2 = ((float) M_PI) * t; /* half angular frequency */
	float m_f = powf(M_MNTS, f);
	float out = sinf(n * wt_2) * cosf((n + 1.0f) * wt_2) / sinf(wt_2);
	/* adjust top harmonics such that new harmonics gradually rise from
 	 * 0 */
	out -= m_f * M_MNTS_NSQR
 	       * (cosf(2.0f * wt_2)
    	          - powf(m_f, n) * cosf((n + 1.0f) * 2.0f * wt_2)
    	          + powf(m_f, n + 1.0f) * cosf(n * 2.0f * wt_2)
    	          - m_f)
 	       / (1.0f + m_f * m_f - 2.0f * m_f * cosf(2.0f * wt_2));
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
