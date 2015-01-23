/** @file cosine.h
 *
 * Cosine Wave Synth
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
#ifndef SONICMATHS_COSINE_H
#define SONICMATHS_COSINE_H 1

#include <math.h>
#include <sonicmaths/oscillator.h>

static inline float smcos(struct smosc *osc, float freq, float phase) {
	float ret;
	ret = cosf((float) (2 * M_PI * (osc->t + (double) phase)));
	osc->t += (double) freq;
	osc->t -= floor(osc->t);
	return ret;
}

#endif /* ! SONICMATHS_COSINE_H */
