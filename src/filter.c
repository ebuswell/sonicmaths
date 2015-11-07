/*
 * filter.c
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
#include "sonicmaths/filter.h"

void smf1low(float *u, int n, float *y, float *x, float *f) {
	int i;
	for (i = 0; i < n; i++) {
		y[i] = smf1lowv(u, x[i], smff2w_2(f[i]));
	}
}

void smf1high(float *u, int n, float *y, float *x, float *f) {
	int i;
	for (i = 0; i < n; i++) {
		y[i] = smf1highv(u, x[i], smff2w_2(f[i]));
	}
}

void smf2low(float *u, int n, float *y, float *x, float *f, float *r) {
	int i;
	for (i = 0; i < n; i++) {
		y[i] = smf2lowv(u, x[i], smff2w_2(f[i]),
				SMF_BWP21 * (1 - r[i]));
	}
}

void smf2high(float *u, int n, float *y, float *x, float *f, float *r) {
	int i;
	for (i = 0; i < n; i++) {
		y[i] = smf2highv(u, x[i], smff2w_2(f[i]),
				 SMF_BWP21 * (1 - r[i]));
	}
}

void smf2band(float *u, int n, float *y, float *x, float *f, float *r) {
	int i;
	for (i = 0; i < n; i++) {
		y[i] = smf2bandv(u, x[i], smff2w_2(f[i]),
				 SMF_BWP21 * (1 - r[i]));
	}
}

void smf3low(float *u, int n, float *y, float *x, float *f, float *r) {
	int i;
	float t, w_2;
	for (i = 0; i < n; i++) {
		w_2 = smff2w_2(f[i]);
		t = smf1lowv(u, x[i], w_2);
		y[i] = smf2lowv(u+1, t, w_2,
				SMF_BWP31 * (1 - r[i]));
	}
}

void smf3high(float *u, int n, float *y, float *x, float *f, float *r) {
	int i;
	float t, w_2;
	for (i = 0; i < n; i++) {
		w_2 = smff2w_2(f[i]);
		t = smf1highv(u, x[i], w_2);
		y[i] = smf2highv(u+1, t, w_2,
				 SMF_BWP31 * (1 - r[i]));
	}
}


void smf4low(float *u, int n, float *y, float *x, float *f, float *r) {
	int i;
	float t, w_2;
	for (i = 0; i < n; i++) {
		w_2 = smff2w_2(f[i]);
		t = smf2lowv(u, x[i], w_2, SMF_BWP42);
		y[i] = smf2lowv(u+2, t, w_2,
				SMF_BWP41 * (1 - r[i]));
	}
}

void smf4high(float *u, int n, float *y, float *x, float *f, float *r) {
	int i;
	float t, w_2;
	for (i = 0; i < n; i++) {
		w_2 = smff2w_2(f[i]);
		t = smf2highv(u, x[i], w_2, SMF_BWP42);
		y[i] = smf2highv(u+2, t, w_2,
				 SMF_BWP41 * (1 - r[i]));
	}
}

void smf4band(float *u, int n, float *y, float *x, float *f, float *r) {
	int i;
	float t, w_2;
	for (i = 0; i < n; i++) {
		w_2 = smff2w_2(f[i]);
		t = smf2bandv(u, x[i], w_2, SMF_BWP42);
		y[i] = smf2bandv(u+2, t, w_2,
				 SMF_BWP41 * (1 - r[i]));
	}
}

void smf6band(float *u, int n, float *y, float *x, float *f, float *r) {
	int i;
	float t, w_2;
	for (i = 0; i < n; i++) {
		w_2 = smff2w_2(f[i]);
		t = smf2bandv(u, x[i], w_2, SMF_BWP63);
		t = smf2bandv(u+2, t, w_2, SMF_BWP62);
		y[i] = smf2bandv(u+4, t, w_2,
				 SMF_BWP61 * (1 - r[i]));
	}
}

void smf8band(float *u, int n, float *y, float *x, float *f, float *r) {
	int i;
	float t, w_2;
	for (i = 0; i < n; i++) {
		w_2 = smff2w_2(f[i]);
		t = smf2bandv(u, x[i], w_2, SMF_BWP84);
		t = smf2bandv(u+2, t, w_2, SMF_BWP83);
		t = smf2bandv(u+4, t, w_2, SMF_BWP82);
		y[i] = smf2bandv(u+6, t, w_2,
				 SMF_BWP81 * (1 - r[i]));
	}
}

void smf4split(float *u, int n, float **y, float *x, float *bw) {
	int i, j;
	float f, _bw, t;
	for (i = 0; i < n; i++) {
		_bw = bw[i];
		t = x[i];
		for (j = 0, f = _bw; f < 0.5f; f += _bw, j++) {
			smf4linkwitz_rileyv(u+6*j, &y[j][i], &t, t,
					    smff2w_2(f));
		}
		y[j][i] = t;
	}
}

void smf3lowres(float *u, int n, float *y, float *x, float *f, float *r) {
	int i;
	float t1, t2, t3, t4, t5, t6, u1, u2, u3, w_2, _r;
	u1 = u[0];
	u2 = u[1];
	u3 = u[2];
	for (i = 0; i < n; i++) {
		w_2 = smff2w_2(f[i]);
		_r = 3.0f * r[i];
		t1 = ((1.0f + w_2) * (1.0f + SMF_BWP31 * w_2 + w_2 * w_2) * x[i]
		      - (w_2 * w_2 * u1 + (1.0f + w_2) * w_2 * u2
			 + (1.0f + w_2) * (1.0f + SMF_BWP31 * w_2) * u3) * _r)
		 / /*-----------------------------------------------------------*/
		     ((1.0f + w_2) * (1.0f + SMF_BWP31 * w_2 + w_2 * w_2)
		      + w_2 * w_2 * w_2 * _r);
		t2 = (t1 - u1) / (1.0f + w_2);
		t3 = u1 + w_2 * t2;
		t4 = (t3 - (w_2 + SMF_BWP31) * u2 - u3)
		 / /*------------------------------------*/
		     (1.0f + SMF_BWP31 * w_2 + w_2 * w_2);
		t5 = u2 + w_2 * t4;
		t6 = u3 + w_2 * t5;
		u1 = w_2 * t2 + t3;
		u2 = w_2 * t4 + t5;
		u3 = w_2 * t5 + t6;
		u1 = SMFPNORM(u1);
		u2 = SMFPNORM(u2);
		u3 = SMFPNORM(u3);
		y[i] = t6;
	}
	u[0] = u1;
	u[1] = u2;
	u[2] = u3;
}

void smf4lowres(float *u, int n, float *y, float *x, float *f, float *r) {
	int i;
	float t1, t2, t3, t4, t5, t6, t7, u1, u2, u3, u4, w_2, _r;
	u1 = u[0];
	u2 = u[1];
	u3 = u[2];
	u4 = u[3];
	for (i = 0; i < n; i++) {
		w_2 = smff2w_2(f[i]);
		_r = ((float) M_SQRT2) * r[i];
		t1 = ((1 + SMF_BWP42 * w_2 + w_2 * w_2)
		       * (1 + SMF_BWP41 * w_2 + w_2 * w_2)
		       * x[i]
		      - (w_2 * w_2 * u1
			 + (1.0f + SMF_BWP42 * w_2) * w_2 * w_2 * u2
			 + (1.0f + SMF_BWP42 * w_2 + w_2 * w_2) * w_2 * u3
			 + (1.0f + SMF_BWP41 * w_2)
			   * (1.0f + SMF_BWP42 * w_2 + w_2 * w_2)
			   * u4)
		        * _r)
		  / /*----------------------------------------------------------*/
		      ((1.0f + SMF_BWP42 * w_2 + w_2 * w_2)
			* (1.0f + SMF_BWP41 * w_2 + w_2 * w_2)
		       + w_2 * w_2 * w_2 * w_2 * _r);
		t2 = (t1 - (w_2 + SMF_BWP42) * u1 - u2)
		 / /*----------------------------*/
		     (1.0f + SMF_BWP42 * w_2 + w_2 * w_2);
		t3 = u1 + t2 * w_2;
		t4 = u2 + t3 * w_2;
		t5 = (t4 - (w_2 + SMF_BWP41) * u3 - u4)
		 / /*----------------------------*/
		     (1.0f + SMF_BWP41 * w_2 + w_2 * w_2);
		t6 = u3 + w_2 * t5;
		t7 = u4 + w_2 * t6;
		u1 = w_2 * t2 + t3;
		u2 = w_2 * t3 + t4;
		u3 = w_2 * t5 + t6;
		u4 = w_2 * t6 + t7;
		u1 = SMFPNORM(u1);
		u2 = SMFPNORM(u2);
		u3 = SMFPNORM(u3);
		u4 = SMFPNORM(u4);
		y[i] = t7;
	}
	u[0] = u1;
	u[1] = u2;
	u[2] = u3;
	u[3] = u4;
}
