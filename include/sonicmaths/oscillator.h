/** @file oscillator.h
 *
 * Structure for generic oscillator functions
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
#ifndef SONICMATHS_OSCILLATOR_H
#define SONICMATHS_OSCILLATOR_H 1

#include <sonicmaths/math.h>

/**
 * Structure for generic oscillator functions
 */
struct smosc {
	double t; /** Current time offset of the wave */
};

/**
 * Initialize oscillator
 */
int smosc_init(struct smosc *osc);

/**
 * Destroy oscillator
 */
void smosc_destroy(struct smosc *osc);

static inline void smosc_set_phase(struct smosc *osc, float phase) {
	osc->t = (double) phase;
}

static inline float smosc_get_phase(struct smosc *osc) {
	return (float) osc->t;
}

#endif /* ! SONICMATHS_SYNTH_H */
