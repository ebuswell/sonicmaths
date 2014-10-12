/** @file sine.h
 *
 * Sine Wave Synth
 *
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
#ifndef SONICMATHS_SINE_H
#define SONICMATHS_SINE_H 1

#include <math.h>
#include <sonicmaths/synth.h>

/**
 * Generate a sine wave with the given frequency and phase.
 */
static inline float smsine(struct smsynth *synth, int channel,
                           float freq, float phase) {
	float ret = sinf(2 * ((float) M_PI) * (synth->t[channel] + phase));
	synth->t[channel] += freq;
	return ret;
}

#endif /* ! SONICMATHS_SINE_H */
