/** @file filter.h
 *
 * Generic first and second order filter stages.
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
#ifndef SONICMATHS_FILTER_H
#define SONICMATHS_FILTER_H 1

#include <sonicmaths/math.h>
#include <math.h>

#define SMF_BWP21	 1.414213562373095f
#define SMF_BWP31	1.0f
#define SMF_BWP41	0.7653668647301796f
#define SMF_BWP42	 1.847759065022574f
#define SMF_BWP61	0.5176380902050414f
#define SMF_BWP62	 1.414213562373095f
#define SMF_BWP63	 1.931851652578136f
#define SMF_BWP81	0.3901806440322565f
#define SMF_BWP82	 1.111140466039204f
#define SMF_BWP83	 1.662939224605090f
#define SMF_BWP84	 1.961570560806461f

#define SMF_BWQ		0.7071067811865475f

#define SMF_FMAX	0.3195f
#define SMF_FMIN	0.0001f

static inline float smff2w_2(float f) {
	return tanf((f > SMF_FMAX ? SMF_FMAX : f < SMF_FMIN ? SMF_FMIN : f)
		    * (float) M_PI);
}

static inline float smf1lowv(float *u, float x, float w_2) {
	float t1, t2, u1;
	u1 = u[0];
	t1 = (x - u1) / (1.0f + w_2);
	t2 = u1 + w_2 * t1;
	u1 = w_2 * t1 + t2;
	u[0] = SMFPNORM(u1);
	return t2;
}

static inline float smf1highv(float *u, float x, float w_2) {
	float t1, t2, u1;
	u1 = u[0];
	t1 = (x - u1) / (1.0f + w_2);
	t2 = u1 + w_2 * t1;
	u1 = w_2 * t1 + t2;
	u[0] = SMFPNORM(u1);
	return t1;
}

static inline void smf1splitv(float *u, float *low, float *high, float x,
			      float w_2) {
	float t1, t2, u1;
	u1 = u[0];
	t1 = (x - u1) / (1.0f + w_2);
	t2 = u1 + w_2 * t1;
	u1 = w_2 * t1 + t2;
	u[0] = SMFPNORM(u1);
	*low = t2;
	*high = t1;
}

static inline float smf2lowv(float *u, float x, float w_2, float a) {
	float t1, t2, t3, u1, u2;
	u1 = u[0];
	u2 = u[1];
	t1 = (x - (w_2 + a) * u1 - u2) / (1.0f + a * w_2 + w_2 * w_2);
	t2 = u1 + w_2 * t1;
	t3 = u2 + w_2 * t2;
	u1 = w_2 * t1 + t2;
	u2 = w_2 * t2 + t3;
	u[0] = SMFPNORM(u1);
	u[1] = SMFPNORM(u2);
	return t3;
}

static inline float smf2highv(float *u, float x, float w_2, float a) {
	float t1, t2, t3, u1, u2;
	u1 = u[0];
	u2 = u[1];
	t1 = (x - (w_2 + a) * u1 - u2) / (1.0f + a * w_2 + w_2 * w_2);
	t2 = u1 + w_2 * t1;
	t3 = u2 + w_2 * t2;
	u1 = w_2 * t1 + t2;
	u2 = w_2 * t2 + t3;
	u[0] = SMFPNORM(u1);
	u[1] = SMFPNORM(u2);
	return t1;
}

static inline float smf2bandv(float *u, float x, float w_2, float a) {
	float t1, t2, t3, u1, u2;
	u1 = u[0];
	u2 = u[1];
	t1 = (x - (w_2 + a) * u1 - u2) / (1.0f + a * w_2 + w_2 * w_2);
	t2 = u1 + w_2 * t1;
	t3 = u2 + w_2 * t2;
	u1 = w_2 * t1 + t2;
	u2 = w_2 * t2 + t3;
	u[0] = SMFPNORM(u1);
	u[1] = SMFPNORM(u2);
	return t2;
}

static inline void smf2splitv(float *u, float *low, float *high, float x,
			      float w_2, float a) {
	float t1, t2, t3, u1, u2;
	u1 = u[0];
	u2 = u[1];
	t1 = (x - (w_2 + a) * u1 - u2) / (1.0f + a * w_2 + w_2 * w_2);
	t2 = u1 + w_2 * t1;
	t3 = u2 + w_2 * t2;
	u1 = w_2 * t1 + t2;
	u2 = w_2 * t2 + t3;
	u[0] = SMFPNORM(u1);
	u[1] = SMFPNORM(u2);
	*low = t3;
	*high = t1;
}

void smf1low(float *u, int n, float *y, float *x, float *f);
void smf1high(float *u, int n, float *y, float *x, float *f);
void smf2low(float *u, int n, float *y, float *x, float *f, float *r);
void smf2high(float *u, int n, float *y, float *x, float *f, float *r);
void smf2band(float *u, int n, float *y, float *x, float *f, float *r);
void smf3low(float *u, int n, float *y, float *x, float *f, float *r);
void smf3high(float *u, int n, float *y, float *x, float *f, float *r);
void smf4low(float *u, int n, float *y, float *x, float *f, float *r);
void smf4high(float *u, int n, float *y, float *x, float *f, float *r);
void smf4band(float *u, int n, float *y, float *x, float *f, float *r);
void smf6band(float *u, int n, float *y, float *x, float *f, float *r);
void smf8band(float *u, int n, float *y, float *x, float *f, float *r);

/* requires u[6] */
static inline void smf4linkwitz_rileyv(float *u, float *low, float *high, float x, float w_2) {
	float t1, t2;
	smf2splitv(u, &t1, &t2, x, w_2, SMF_BWP21);
	*low = smf2lowv(u+2, t1, w_2, SMF_BWP21);
	*high = smf2highv(u+4, t2, w_2, SMF_BWP21);
}

/* requires u[6] multiplied by 1/(2 bw) */
void smf4split(float *u, int n, float **y, float *x, float *bw);

void smf3lowres(float *u, int n, float *y, float *x, float *f, float *r);
void smf4lowres(float *u, int n, float *y, float *x, float *f, float *r);

#endif /* ! SONICMATHS_FILTER_H */
