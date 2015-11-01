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

static inline float smff2fw(float f) {
	return tanf((f > 0.499f ? 0.499f : f) * (float) M_PI);
}

static inline float smf1l(float x, float x1, float y1, float w) {
	float y;
	y = (w * (x + x1) - (w - 1.0f) * y1) / (w + 1.0f);
	return SMFPNORM(y);
}

static inline float smf1h(float x, float x1, float y1, float w) {
	float y;
	y = ((x - x1) - (w - 1.0f) * y1) / (w + 1.0f);
	return SMFPNORM(y);
}

static inline float smf2l(float x, float x1, float x2,
			  float y1, float y2, float w, float a) {
	float y;
	float w2;
	float aw;
	w2 = w * w;
	aw = a * w;
	y = (w2 * (x + 2.0f * x1 + x2)
	     - 2.0f * (w2 - 1.0f) * y1
	     - (w2 - aw + 1.0f) * y2)
	    / (1 + aw + w2);
	return SMFPNORM(y);
}

static inline float smf2h(float x, float x1, float x2,
			  float y1, float y2, float w, float a) {
	float y;
	float w2;
	float aw;
	w2 = w * w;
	aw = a * w;
	y = ((x - 2.0f * x1 + x2)
	     - 2.0f * (w2 - 1.0f) * y1
	     - (w2 - aw + 1.0f) * y2)
	    / (1 + aw + w2);
	return SMFPNORM(y);
}

static inline float smf2p(float x, float x1 __attribute__((unused)), float x2,
			  float y1, float y2, float w, float a) {
	float y;
	float w2;
	float aw;
	w2 = w * w;
	aw = a * w;
	y = (w * (x - x2)
	     - 2.0f * (w2 - 1.0f) * y1
	     - (w2 - aw + 1.0f) * y2)
	    / (1 + aw + w2);
	return SMFPNORM(y);
}

static inline float smf2s(float x, float x1, float x2,
			  float y1, float y2, float w, float a) {
	float y;
	float w2;
	float aw;
	w2 = w * w;
	aw = a * w;
	y = ((w2 + 1.0f) * (x + x2)
	     + (w2 - 1.0f) * (x1 - y1)
	     - (w2 - aw + 1.0f) * y2)
	    / (1 + aw + w2);
	return SMFPNORM(y);
}

struct smf1stage {
	float x1;
	float y1;
};

struct smf2stage {
	float x1;
	float x2;
	float y1;
	float y2;
};

struct smf1o {
	struct smf1stage s;
};

struct smf2o {
	struct smf2stage s;
};

struct smf3o {
	struct smf1stage s1;
	struct smf2stage s2;
};

struct smf4o {
	struct smf2stage s1;
	struct smf2stage s2;
};

struct smf6o {
	struct smf2stage s1;
	struct smf2stage s2;
	struct smf2stage s3;
};

struct smf8o {
	struct smf2stage s1;
	struct smf2stage s2;
	struct smf2stage s3;
	struct smf2stage s4;
};

int smf1o_init(struct smf1o *filter);
void smf1o_destroy(struct smf1o *filter);
int smf2o_init(struct smf2o *filter);
void smf2o_destroy(struct smf2o *filter);
int smf3o_init(struct smf3o *filter);
void smf3o_destroy(struct smf3o *filter);
int smf4o_init(struct smf4o *filter);
void smf4o_destroy(struct smf4o *filter);
int smf6o_init(struct smf6o *filter);
void smf6o_destroy(struct smf6o *filter);
int smf8o_init(struct smf8o *filter);
void smf8o_destroy(struct smf8o *filter);

void smflp1(struct smf1o *filter, int n, float *y, float *x, float *f);
void smfhp1(struct smf1o *filter, int n, float *y, float *x, float *f);
void smflp2(struct smf2o *filter, int n, float *y, float *x, float *f, float *Q);
void smfhp2(struct smf2o *filter, int n, float *y, float *x, float *f, float *Q);
void smfbp2(struct smf2o *filter, int n, float *y, float *x, float *f, float *Q);
void smfbs2(struct smf2o *filter, int n, float *y, float *x, float *f, float *Q);
void smflp3(struct smf3o *filter, int n, float *y, float *x, float *f, float *Q);
void smfhp3(struct smf3o *filter, int n, float *y, float *x, float *f, float *Q);
void smflp4(struct smf4o *filter, int n, float *y, float *x, float *f, float *Q);
void smfhp4(struct smf4o *filter, int n, float *y, float *x, float *f, float *Q);
void smfbp4(struct smf4o *filter, int n, float *y, float *x, float *f, float *Q);
void smfbs4(struct smf4o *filter, int n, float *y, float *x, float *f, float *Q);
void smfbp6(struct smf6o *filter, int n, float *y, float *x, float *f, float *Q);
void smfbs6(struct smf6o *filter, int n, float *y, float *x, float *f, float *Q);
void smfbp8(struct smf8o *filter, int n, float *y, float *x, float *f, float *Q);
void smfbs8(struct smf8o *filter, int n, float *y, float *x, float *f, float *Q);

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

#endif /* ! SONICMATHS_FILTER_H */
