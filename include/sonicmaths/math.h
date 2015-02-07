/** @file math.h
 *
 * Miscellaneous mathematical functions for Sonic Maths.
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
#ifndef SONICMATHS_MATH_H
#define SONICMATHS_MATH_H 1

#include <stdlib.h>
#include <math.h>
#include <float.h>

/* e^-pi, the base of the exponent for exponential decays. */
#define EXP_NEG_PI 0.043213918263772250f

static inline float smaths_normtime(float sample_rate, float t) {
	return t * sample_rate;
}

static inline float smaths_normfreq(float sample_rate, float f) {
	return f / sample_rate;
}

#define SMNORM(x) ((isnormal(x) || x == 0.0f) ? x	\
		   : x == INFINITY ? FLT_MAX		\
		   : x == -INFINITY ? -FLT_MAX		\
		   : 0.0f)

static inline float smaths_wsinc(float x, float n) {
	float w1;
	float w2;
	w1 = (float) M_PI * (x - n / 2.0f);
	if (w1 == 0.0f) {
		return 1.0f;
	}
	w2 = 2.0f * w1 / n;
	return (0.42f + 0.5f * cosf(w2) + 0.08f * cosf(2.0f * w2))
		* sinf(w1) / w1;
}

/* See:
 * https://web.archive.org/web/20060708031958/http://www.biochem.oulu.fi/~oniemita/dsp/hilbert/
 */

struct smhilbert_pair_sect {
	float x1;
	float x2;
	float y1;
	float y2;
};

struct smhilbert_pair {
	struct smhilbert_pair_sect sect[2][4];
	float y1;
};

static inline float smhilbert_pair_sect(struct smhilbert_pair_sect *sect,
				        float x, float a_2) {
	float y;
	y = a_2 * (x + sect->y2) - sect->x2;
	sect->x2 = sect->x1;
	sect->x1 = x;
	sect->y2 = sect->y1;
	sect->y1 = SMNORM(y);
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

static inline void smhilbert_pair(struct smhilbert_pair *coeff, float x,
				  float *y, float *y_pi_2) {
	*y_pi_2 = smhilbert_pair_sect(&coeff->sect[1][0], x, SMA_2_10);
	*y_pi_2 = smhilbert_pair_sect(&coeff->sect[1][1], *y_pi_2, SMA_2_11);
	*y_pi_2 = smhilbert_pair_sect(&coeff->sect[1][2], *y_pi_2, SMA_2_12);
	*y_pi_2 = smhilbert_pair_sect(&coeff->sect[1][3], *y_pi_2, SMA_2_12);
	x = smhilbert_pair_sect(&coeff->sect[0][0], x, SMA_2_00);
	x = smhilbert_pair_sect(&coeff->sect[0][1], x, SMA_2_01);
	x = smhilbert_pair_sect(&coeff->sect[0][2], x, SMA_2_02);
	x = smhilbert_pair_sect(&coeff->sect[0][3], x, SMA_2_02);
	*y = coeff->y1;
	coeff->y1 = x;
}

int smhilbert_pair_init(struct smhilbert_pair *coeff);
void smhilbert_pair_destroy(struct smhilbert_pair *coeff);

#endif /* ! SONICMATHS_MATH_H */
