/*
 * cosine.c
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
#include "sonicmaths/oscillator.h"
#include "sonicmaths/cosine.h"

void smcos(struct smosc *osc, int n, float *y, float *f, float *phi) {
	int i;
	double t;
	t = osc->t;
	for (i = 0; i < n; i++) {
		y[i] = cosf(((float) (2 * M_PI)) * (((float) t) + phi[i]));
		t += (double) f[i];
		t -= floor(t);
	}
	osc->t = isfinite(t) ? t : 0.0;
}
