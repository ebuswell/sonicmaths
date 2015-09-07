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

#define A 16.080840619878092f

void smitrain(struct smosc *osc, int n, float *y, float *f, float *phi) {
	int i;
	double t;
	float _y, _f, f_2, wt_2, nh,
	      sinw, cosw, sinnw, cosnw, sinn1w, cosn1w,
	      sinhaf, coshaf, sinhnaf, coshnaf;
	t = osc->t;
	for (i = 0; i < n; i++) {
		_f = f[i];
		nh = floorf(1.0f / (2.0f * _f)); /* the number of harmonics */
		f_2 = _f * 0.5f;
		wt_2 = ((float) M_PI) * (((float) t) + phi[i]);
		sinw = sinf(wt_2);
		cosw = cosf(wt_2);
		sinnw = sinf(nh * wt_2);
		cosnw = cosf(nh * wt_2);
		sinn1w = sinf((nh + 1.0f) * wt_2);
		cosn1w = cosf((nh + 1.0f) * wt_2);
		sinhaf = sinh(A * f_2);
		coshaf = cosh(A * f_2);
		sinhnaf = sinh(nh * A * f_2);
		coshnaf = cosh(nh * A * f_2);
		_y = cosn1w * sinnw / sinw;
		_y -= expf(A * ((nh + 1.0f) * f_2 - 0.5f))
		      * (  cosn1w * (sinhnaf * sinhaf * cosnw * cosw
				     + coshnaf * coshaf * sinnw * sinw)
			 + sinn1w * (sinhnaf * coshaf * cosnw * sinw
				     - coshnaf * sinhaf * sinnw * cosw))
		      / (sinhaf * sinhaf + sinw * sinw);
		y[i] = _y;
		t += (double) _f;
		t -= floor(t);
	}
	osc->t = isfinite(t) ? t : 0.0;
}
