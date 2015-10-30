/*
 * impulse-train.c
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
#include "sonicmaths/impulse-train.h"

void smitrain(struct smosc *osc, int n, float *y, float *f, float *phi) {
	int i;
	double t;
	float _y, _f, wt_2, nh, ha,
	      sinw_2, sinnw_2, cosn1w_2, cosn1w;
	t = osc->t;
	for (i = 0; i < n; i++) {
		_f = f[i];
		ha = 1.0f / (2.0f * _f);
		nh = floorf(ha); /* the number of harmonics */
		ha -= nh; /* the strength of the top harmonic */
		nh -= 1.0f;
		wt_2 = ((float)  M_PI) * (((float) t) + phi[i]);
		sinw_2 = sinf(wt_2);
		sinnw_2 = sinf(nh * wt_2);
		cosn1w_2 = cosf((nh + 1.0f) * wt_2);
		cosn1w = 2.0f * cosn1w_2 * cosn1w_2 - 1.0f;
		_y = cosn1w_2 * sinnw_2 / sinw_2;
		_y += ha * cosn1w;
		y[i] = _y;
		t += (double) _f;
		t -= floor(t);
	}
	osc->t = isfinite(t) ? t : 0.0;
}
