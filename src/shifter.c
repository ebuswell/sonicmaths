/*
 * shifter.c
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
#include <string.h>
#include <math.h>
#include "sonicmaths/math.h"
#include "sonicmaths/oscillator.h"
#include "sonicmaths/filter.h"
#include "sonicmaths/shifter.h"

/* See:
 * https://web.archive.org/web/20060708031958/http://www.biochem.oulu.fi/~oniemita/dsp/hilbert/
 */

static inline float smhilbert_pair_sect(struct smhilbert_pair_sect *sect,
				        float x, float a_2) {
	float y;
	y = a_2 * (x + sect->y2) - sect->x2;
	sect->x2 = sect->x1;
	sect->x1 = x;
	sect->y2 = sect->y1;
	sect->y1 = y;
	return y;
}

#define SMA_2_00 0.4794008655888399
#define SMA_2_01 0.87621849353931 
#define SMA_2_02 0.976597589508199
#define SMA_2_03 0.997499255935549
#define SMA_2_10 0.1617584983677011
#define SMA_2_11 0.7330289323414904
#define SMA_2_12 0.945349700329113
#define SMA_2_13 0.990599156684529

int smshift_init(struct smshift *shift) {
	int r;
	r = smosc_init(&shift->osc);
	if (r != 0) {
		return r;
	}
	memset(&shift->coeff, 0, sizeof(struct smhilbert_pair));
	r = smf2o_init(&shift->filter);
	if (r != 0) {
		smosc_destroy(&shift->osc);
		return r;
	}
	return 0;
}

void smshift_destroy(struct smshift *shift) {
	smosc_destroy(&shift->osc);
	smf2o_destroy(&shift->filter);
}

void smshift(struct smshift *shift, int n, float *y, float *x, float *f) {
	float s, c, _y, y1, y_pi_2, _x;
	double t;
	int i;
	float lpf[n];
	float lpQ[n];
	for (i = 0; i < n; i++) {
		lpf[i] = f[i] > 0 ? 0.9995f * (0.5f - f[i])
				  : 0.9995f * 0.5f;
		lpQ[i] = SMF_BUTTERWORTH_Q;
	}
	smflp2(&shift->filter, n, x, x, lpf, lpQ);
	t = shift->osc.t;
	y1 = shift->coeff.y1;
	for (i = 0; i < n; i++) {
		s = sinf(2 * M_PI * t);
		c = cosf(2 * M_PI * t);
		t += (double) f[i];
		t -= floor(t);

		_x = x[i];
		_y = y1;
		y_pi_2 = smhilbert_pair_sect(&shift->coeff.sect[1][0], _x, SMA_2_10);
		y_pi_2 = smhilbert_pair_sect(&shift->coeff.sect[1][1], y_pi_2, SMA_2_11);
		y_pi_2 = smhilbert_pair_sect(&shift->coeff.sect[1][2], y_pi_2, SMA_2_12);
		y_pi_2 = smhilbert_pair_sect(&shift->coeff.sect[1][3], y_pi_2, SMA_2_12);
		y1 = smhilbert_pair_sect(&shift->coeff.sect[0][0], _x, SMA_2_00);
		y1 = smhilbert_pair_sect(&shift->coeff.sect[0][1], y1, SMA_2_01);
		y1 = smhilbert_pair_sect(&shift->coeff.sect[0][2], y1, SMA_2_02);
		y1 = smhilbert_pair_sect(&shift->coeff.sect[0][3], y1, SMA_2_02);

		y[i] = _y * c + y_pi_2 * s;
	}
	shift->osc.t = t;
}
