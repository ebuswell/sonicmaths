/*
 * clock.c
 *
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
#include <math.h>
#include "sonicmaths/clock.h"

int smclock_init(struct smclock *clock) {
	clock->t = 0.0;
	return 0;
}

void smclock_destroy(struct smclock *clock __attribute__((unused))) {
	/* Do nothing */
}

void smclock(struct smclock *clock, int n, float *y, float *rate) {
	int i;
	double t = clock->t;
	for (i = 0; i < n; i++) {
		y[i] = t;
		t += (double) rate[i];
	}
	clock->t = isfinite(t) ? t
		 : isnan(t) ? 0.0
		 : t > 0 ? HUGE_VAL
		 : -HUGE_VAL;
}
