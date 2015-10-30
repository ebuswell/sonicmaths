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
#include <string.h>
#include "sonicmaths/filter.h"

int smf1o_init(struct smf1o *filter) {
	memset(filter, 0, sizeof(struct smf1o));
	return 0;
}

void smf1o_destroy(struct smf1o *filter __attribute__((unused))) {
	/* Do nothing */
}

int smf2o_init(struct smf2o *filter) {
	memset(filter, 0, sizeof(struct smf2o));
	return 0;
}

void smf2o_destroy(struct smf2o *filter __attribute__((unused))) {
	/* Do nothing */
}

int smf3o_init(struct smf3o *filter) {
	memset(filter, 0, sizeof(struct smf3o));
	return 0;
}

void smf3o_destroy(struct smf3o *filter __attribute__((unused))) {
	/* Do nothing */
}

int smf4o_init(struct smf4o *filter) {
	memset(filter, 0, sizeof(struct smf4o));
	return 0;
}

void smf4o_destroy(struct smf4o *filter __attribute__((unused))) {
	/* Do nothing */
}

int smf6o_init(struct smf6o *filter) {
	memset(filter, 0, sizeof(struct smf6o));
	return 0;
}

void smf6o_destroy(struct smf6o *filter __attribute__((unused))) {
	/* Do nothing */
}

int smf8o_init(struct smf8o *filter) {
	memset(filter, 0, sizeof(struct smf8o));
	return 0;
}

void smf8o_destroy(struct smf8o *filter __attribute__((unused))) {
	/* Do nothing */
}

void smflp1(struct smf1o *filter, int n, float *y, float *x, float *f) {
	int i;
	float _y, y1, _x, x1;
	x1 = filter->s.x1;
	y1 = filter->s.y1;
	for (i = 0; i < n; i++) {
		_x = x[i];
		_y = smf1l(_x, x1, y1, smff2fw(f[i]));
		y1 = _y;
		x1 = _x;
		y[i] = _y;
	}
	filter->s.x1 = x1;
	filter->s.y1 = y1;
}

void smfhp1(struct smf1o *filter, int n, float *y, float *x, float *f) {
	int i;
	float _y, y1, _x, x1;
	x1 = filter->s.x1;
	y1 = filter->s.y1;
	for (i = 0; i < n; i++) {
		_x = x[i];
		_y = smf1h(_x, x1, y1, smff2fw(f[i]));
		y1 = _y;
		x1 = _x;
		y[i] = _y;
	}
	filter->s.x1 = x1;
	filter->s.y1 = y1;
}

void smflp2(struct smf2o *filter, int n, float *y, float *x, float *f, float *Q) {
	int i;
	float _y, y1, y2, _x, x1, x2;
	x1 = filter->s.x1;
	x2 = filter->s.x2;
	y1 = filter->s.y1;
	y2 = filter->s.y2;
	for (i = 0; i < n; i++) {
		_x = x[i];
		_y = smf2l(_x, x1, x2, y1, y2, smff2fw(f[i]), 1.0f/Q[i]);
		y2 = y1;
		y1 = _y;
		x2 = x1;
		x1 = _x;
		y[i] = _y;
	}
	filter->s.x1 = x1;
	filter->s.x2 = x2;
	filter->s.y1 = y1;
	filter->s.y2 = y2;
}

void smfhp2(struct smf2o *filter, int n, float *y, float *x, float *f, float *Q) {
	int i;
	float _y, y1, y2, _x, x1, x2;
	x1 = filter->s.x1;
	x2 = filter->s.x2;
	y1 = filter->s.y1;
	y2 = filter->s.y2;
	for (i = 0; i < n; i++) {
		_x = x[i];
		_y = smf2h(_x, x1, x2, y1, y2, smff2fw(f[i]), 1.0f/Q[i]);
		y2 = y1;
		y1 = _y;
		x2 = x1;
		x1 = _x;
		y[i] = _y;
	}
	filter->s.x1 = x1;
	filter->s.x2 = x2;
	filter->s.y1 = y1;
	filter->s.y2 = y2;
}

void smfbp2(struct smf2o *filter, int n, float *y, float *x, float *f, float *Q) {
	int i;
	float _y, y1, y2, _x, x1, x2;
	x1 = filter->s.x1;
	x2 = filter->s.x2;
	y1 = filter->s.y1;
	y2 = filter->s.y2;
	for (i = 0; i < n; i++) {
		_x = x[i];
		_y = smf2p(_x, x1, x2, y1, y2, smff2fw(f[i]), 1.0f/Q[i]);
		y2 = y1;
		y1 = _y;
		x2 = x1;
		x1 = _x;
		y[i] = _y;
	}
	filter->s.x1 = x1;
	filter->s.x2 = x2;
	filter->s.y1 = y1;
	filter->s.y2 = y2;
}

void smfbs2(struct smf2o *filter, int n, float *y, float *x, float *f, float *Q) {
	int i;
	float _y, y1, y2, _x, x1, x2;
	x1 = filter->s.x1;
	x2 = filter->s.x2;
	y1 = filter->s.y1;
	y2 = filter->s.y2;
	for (i = 0; i < n; i++) {
		_x = x[i];
		_y = smf2s(_x, x1, x2, y1, y2, smff2fw(f[i]), 1.0f/Q[i]);
		y2 = y1;
		y1 = _y;
		x2 = x1;
		x1 = _x;
		y[i] = _y;
	}
	filter->s.x1 = x1;
	filter->s.x2 = x2;
	filter->s.y1 = y1;
	filter->s.y2 = y2;
}

void smflp3(struct smf3o *filter, int n, float *y, float *x, float *f, float *Q) {
	int i;
	float _y, s1y1, s2y1, s2y2, _x, s1x1, s2x1, s2x2, w;
	s1x1 = filter->s1.x1;
	s2x1 = filter->s2.x1;
	s2x2 = filter->s2.x2;
	s1y1 = filter->s1.y1;
	s2y1 = filter->s2.y1;
	s2y2 = filter->s2.y2;
	for (i = 0; i < n; i++) {
		_x = x[i];
		w = smff2fw(f[i]);
		_y = smf1l(_x, s1x1, s1y1, w);
		s1y1 = _y;
		s1x1 = _x;
		_x = _y;
		_y = smf2l(_x, s2x1, s2x2, s2y1, s2y2, w, SMF_BUTTERWORTH_P12_INV / Q[i]);
		s2y2 = s2y1;
		s2y1 = _y;
		s2x2 = s2x1;
		s2x1 = _x;
		y[i] = _y;
	}
	filter->s1.x1 = s1x1;
	filter->s2.x1 = s2x1;
	filter->s2.x2 = s2x2;
	filter->s1.y1 = s1y1;
	filter->s2.y1 = s2y1;
	filter->s2.y2 = s2y2;
}

void smfhp3(struct smf3o *filter, int n, float *y, float *x, float *f, float *Q) {
	int i;
	float _y, s1y1, s2y1, s2y2, _x, s1x1, s2x1, s2x2, w;
	s1x1 = filter->s1.x1;
	s2x1 = filter->s2.x1;
	s2x2 = filter->s2.x2;
	s1y1 = filter->s1.y1;
	s2y1 = filter->s2.y1;
	s2y2 = filter->s2.y2;
	for (i = 0; i < n; i++) {
		_x = x[i];
		w = smff2fw(f[i]);
		_y = smf1h(_x, s1x1, s1y1, w);
		s1y1 = _y;
		s1x1 = _x;
		_x = _y;
		_y = smf2h(_x, s2x1, s2x2, s2y1, s2y2, w, SMF_BUTTERWORTH_P12_INV / Q[i]);
		s2y2 = s2y1;
		s2y1 = _y;
		s2x2 = s2x1;
		s2x1 = _x;
		y[i] = _y;
	}
	filter->s1.x1 = s1x1;
	filter->s2.x1 = s2x1;
	filter->s2.x2 = s2x2;
	filter->s1.y1 = s1y1;
	filter->s2.y1 = s2y1;
	filter->s2.y2 = s2y2;
}

void smflp4(struct smf4o *filter, int n, float *y, float *x, float *f, float *Q) {
	int i;
	float _y, s1y1, s1y2, s2y1, s2y2, _x, s1x1, s1x2, s2x1, s2x2, w;
	s1x1 = filter->s1.x1;
	s1x2 = filter->s1.x2;
	s2x1 = filter->s2.x1;
	s2x2 = filter->s2.x2;
	s1y1 = filter->s1.y1;
	s1y2 = filter->s1.y2;
	s2y1 = filter->s2.y1;
	s2y2 = filter->s2.y2;
	for (i = 0; i < n; i++) {
		_x = x[i];
		w = smff2fw(f[i]);
		_y = smf2l(_x, s1x1, s1x2, s1y1, s1y2, w, SMF_BUTTERWORTH_P24);
		s1y2 = s1y1;
		s1y1 = _y;
		s1x2 = s1x1;
		s1x1 = _x;
		_x = _y;
		_y = smf2l(_x, s2x1, s2x2, s2y1, s2y2, w, SMF_BUTTERWORTH_P24_INV / Q[i]);
		s2y2 = s2y1;
		s2y1 = _y;
		s2x2 = s2x1;
		s2x1 = _x;
		y[i] = _y;
	}
	filter->s1.x1 = s1x1;
	filter->s1.x2 = s1x2;
	filter->s2.x1 = s2x1;
	filter->s2.x2 = s2x2;
	filter->s1.y1 = s1y1;
	filter->s1.y2 = s1y2;
	filter->s2.y1 = s2y1;
	filter->s2.y2 = s2y2;
}

void smfhp4(struct smf4o *filter, int n, float *y, float *x, float *f, float *Q) {
	int i;
	float _y, s1y1, s1y2, s2y1, s2y2, _x, s1x1, s1x2, s2x1, s2x2, w;
	s1x1 = filter->s1.x1;
	s1x2 = filter->s1.x2;
	s2x1 = filter->s2.x1;
	s2x2 = filter->s2.x2;
	s1y1 = filter->s1.y1;
	s1y2 = filter->s1.y2;
	s2y1 = filter->s2.y1;
	s2y2 = filter->s2.y2;
	for (i = 0; i < n; i++) {
		_x = x[i];
		w = smff2fw(f[i]);
		_y = smf2h(_x, s1x1, s1x2, s1y1, s1y2, w, SMF_BUTTERWORTH_P24);
		s1y2 = s1y1;
		s1y1 = _y;
		s1x2 = s1x1;
		s1x1 = _x;
		_x = _y;
		_y = smf2h(_x, s2x1, s2x2, s2y1, s2y2, w, SMF_BUTTERWORTH_P24_INV / Q[i]);
		s2y2 = s2y1;
		s2y1 = _y;
		s2x2 = s2x1;
		s2x1 = _x;
		y[i] = _y;
	}
	filter->s1.x1 = s1x1;
	filter->s1.x2 = s1x2;
	filter->s2.x1 = s2x1;
	filter->s2.x2 = s2x2;
	filter->s1.y1 = s1y1;
	filter->s1.y2 = s1y2;
	filter->s2.y1 = s2y1;
	filter->s2.y2 = s2y2;
}

void smfbp4(struct smf4o *filter, int n, float *y, float *x, float *f, float *Q) {
	int i;
	float _y, s1y1, s1y2, s2y1, s2y2, _x, s1x1, s1x2, s2x1, s2x2, w;
	s1x1 = filter->s1.x1;
	s1x2 = filter->s1.x2;
	s2x1 = filter->s2.x1;
	s2x2 = filter->s2.x2;
	s1y1 = filter->s1.y1;
	s1y2 = filter->s1.y2;
	s2y1 = filter->s2.y1;
	s2y2 = filter->s2.y2;
	for (i = 0; i < n; i++) {
		_x = x[i];
		w = smff2fw(f[i]);
		_y = smf2p(_x, s1x1, s1x2, s1y1, s1y2, w, SMF_BUTTERWORTH_P12);
		s1y2 = s1y1;
		s1y1 = _y;
		s1x2 = s1x1;
		s1x1 = _x;
		_x = _y;
		_y = smf2p(_x, s2x1, s2x2, s2y1, s2y2, w, SMF_BUTTERWORTH_P12_INV / Q[i]);
		s2y2 = s2y1;
		s2y1 = _y;
		s2x2 = s2x1;
		s2x1 = _x;
		y[i] = _y;
	}
	filter->s1.x1 = s1x1;
	filter->s1.x2 = s1x2;
	filter->s2.x1 = s2x1;
	filter->s2.x2 = s2x2;
	filter->s1.y1 = s1y1;
	filter->s1.y2 = s1y2;
	filter->s2.y1 = s2y1;
	filter->s2.y2 = s2y2;
}

void smfbs4(struct smf4o *filter, int n, float *y, float *x, float *f, float *Q) {
	int i;
	float _y, s1y1, s1y2, s2y1, s2y2, _x, s1x1, s1x2, s2x1, s2x2, w;
	s1x1 = filter->s1.x1;
	s1x2 = filter->s1.x2;
	s2x1 = filter->s2.x1;
	s2x2 = filter->s2.x2;
	s1y1 = filter->s1.y1;
	s1y2 = filter->s1.y2;
	s2y1 = filter->s2.y1;
	s2y2 = filter->s2.y2;
	for (i = 0; i < n; i++) {
		_x = x[i];
		w = smff2fw(f[i]);
		_y = smf2s(_x, s1x1, s1x2, s1y1, s1y2, w, SMF_BUTTERWORTH_P12);
		s1y2 = s1y1;
		s1y1 = _y;
		s1x2 = s1x1;
		s1x1 = _x;
		_x = _y;
		_y = smf2s(_x, s2x1, s2x2, s2y1, s2y2, w, SMF_BUTTERWORTH_P12_INV / Q[i]);
		s2y2 = s2y1;
		s2y1 = _y;
		s2x2 = s2x1;
		s2x1 = _x;
		y[i] = _y;
	}
	filter->s1.x1 = s1x1;
	filter->s1.x2 = s1x2;
	filter->s2.x1 = s2x1;
	filter->s2.x2 = s2x2;
	filter->s1.y1 = s1y1;
	filter->s1.y2 = s1y2;
	filter->s2.y1 = s2y1;
	filter->s2.y2 = s2y2;
}

void smfbp6(struct smf6o *filter, int n, float *y, float *x, float *f, float *Q) {
	int i;
	float _y, s1y1, s1y2, s2y1, s2y2, s3y1, s3y2,
	      _x, s1x1, s1x2, s2x1, s2x2, s3x1, s3x2, w;
	s1x1 = filter->s1.x1;
	s1x2 = filter->s1.x2;
	s2x1 = filter->s2.x1;
	s2x2 = filter->s2.x2;
	s3x1 = filter->s3.x1;
	s3x2 = filter->s3.x2;
	s1y1 = filter->s1.y1;
	s1y2 = filter->s1.y2;
	s2y1 = filter->s2.y1;
	s2y2 = filter->s2.y2;
	s3y1 = filter->s3.y1;
	s3y2 = filter->s3.y2;
	for (i = 0; i < n; i++) {
		_x = x[i];
		w = smff2fw(f[i]);
		_y = smf2p(_x, s1x1, s1x2, s1y1, s1y2, w, 1.0f);
		s1y2 = s1y1;
		s1y1 = _y;
		s1x2 = s1x1;
		s1x1 = _x;
		_x = _y;
		_y = smf2p(_x, s2x1, s2x2, s2y1, s2y2, w, SMF_BUTTERWORTH_P24);
		s2y2 = s2y1;
		s2y1 = _y;
		s2x2 = s2x1;
		s2x1 = _x;
		_x = _y;
		_y = smf2p(_x, s3x1, s3x2, s3y1, s3y2, w, SMF_BUTTERWORTH_P24_INV / Q[i]);
		s3y2 = s3y1;
		s3y1 = _y;
		s3x2 = s3x1;
		s3x1 = _x;
		y[i] = _y;
	}
	filter->s1.x1 = s1x1;
	filter->s1.x2 = s1x2;
	filter->s2.x1 = s2x1;
	filter->s2.x2 = s2x2;
	filter->s3.x1 = s3x1;
	filter->s3.x2 = s3x2;
	filter->s1.y1 = s1y1;
	filter->s1.y2 = s1y2;
	filter->s2.y1 = s2y1;
	filter->s2.y2 = s2y2;
	filter->s3.y1 = s3y1;
	filter->s3.y2 = s3y2;
}

void smfbs6(struct smf6o *filter, int n, float *y, float *x, float *f, float *Q) {
	int i;
	float _y, s1y1, s1y2, s2y1, s2y2, s3y1, s3y2,
	      _x, s1x1, s1x2, s2x1, s2x2, s3x1, s3x2, w;
	s1x1 = filter->s1.x1;
	s1x2 = filter->s1.x2;
	s2x1 = filter->s2.x1;
	s2x2 = filter->s2.x2;
	s3x1 = filter->s3.x1;
	s3x2 = filter->s3.x2;
	s1y1 = filter->s1.y1;
	s1y2 = filter->s1.y2;
	s2y1 = filter->s2.y1;
	s2y2 = filter->s2.y2;
	s3y1 = filter->s3.y1;
	s3y2 = filter->s3.y2;
	for (i = 0; i < n; i++) {
		_x = x[i];
		w = smff2fw(f[i]);
		_y = smf2s(_x, s1x1, s1x2, s1y1, s1y2, w, 1.0f);
		s1y2 = s1y1;
		s1y1 = _y;
		s1x2 = s1x1;
		s1x1 = _x;
		_x = _y;
		_y = smf2s(_x, s2x1, s2x2, s2y1, s2y2, w, SMF_BUTTERWORTH_P24);
		s2y2 = s2y1;
		s2y1 = _y;
		s2x2 = s2x1;
		s2x1 = _x;
		_x = _y;
		_y = smf2s(_x, s3x1, s3x2, s3y1, s3y2, w, SMF_BUTTERWORTH_P24_INV / Q[i]);
		s3y2 = s3y1;
		s3y1 = _y;
		s3x2 = s3x1;
		s3x1 = _x;
		y[i] = _y;
	}
	filter->s1.x1 = s1x1;
	filter->s1.x2 = s1x2;
	filter->s2.x1 = s2x1;
	filter->s2.x2 = s2x2;
	filter->s3.x1 = s3x1;
	filter->s3.x2 = s3x2;
	filter->s1.y1 = s1y1;
	filter->s1.y2 = s1y2;
	filter->s2.y1 = s2y1;
	filter->s2.y2 = s2y2;
	filter->s3.y1 = s3y1;
	filter->s3.y2 = s3y2;
}

void smfbp8(struct smf8o *filter, int n, float *y, float *x, float *f, float *Q) {
	int i;
	float _y, s1y1, s1y2, s2y1, s2y2, s3y1, s3y2, s4y1, s4y2,
	      _x, s1x1, s1x2, s2x1, s2x2, s3x1, s3x2, s4x1, s4x2, w;
	s1x1 = filter->s1.x1;
	s1x2 = filter->s1.x2;
	s2x1 = filter->s2.x1;
	s2x2 = filter->s2.x2;
	s3x1 = filter->s3.x1;
	s3x2 = filter->s3.x2;
	s4x1 = filter->s4.x1;
	s4x2 = filter->s4.x2;
	s1y1 = filter->s1.y1;
	s1y2 = filter->s1.y2;
	s2y1 = filter->s2.y1;
	s2y2 = filter->s2.y2;
	s3y1 = filter->s3.y1;
	s3y2 = filter->s3.y2;
	s4y1 = filter->s4.y1;
	s4y2 = filter->s4.y2;
	for (i = 0; i < n; i++) {
		_x = x[i];
		w = smff2fw(f[i]);
		_y = smf2p(_x, s1x1, s1x2, s1y1, s1y2, w, SMF_BUTTERWORTH_P24);
		s1y2 = s1y1;
		s1y1 = _y;
		s1x2 = s1x1;
		s1x1 = _x;
		_x = _y;
		_y = smf2p(_x, s2x1, s2x2, s2y1, s2y2, w, SMF_BUTTERWORTH_P24_INV);
		s2y2 = s2y1;
		s2y1 = _y;
		s2x2 = s2x1;
		s2x1 = _x;
		_x = _y;
		_y = smf2p(_x, s3x1, s3x2, s3y1, s3y2, w, SMF_BUTTERWORTH_P48);
		s3y2 = s3y1;
		s3y1 = _y;
		s3x2 = s3x1;
		s3x1 = _x;
		_x = _y;
		_y = smf2p(_x, s4x1, s4x2, s4y1, s4y2, w, SMF_BUTTERWORTH_P48_INV / Q[i]);
		s3y2 = s3y1;
		s3y1 = _y;
		s3x2 = s3x1;
		s3x1 = _x;
		y[i] = _y;
	}
	filter->s1.x1 = s1x1;
	filter->s1.x2 = s1x2;
	filter->s2.x1 = s2x1;
	filter->s2.x2 = s2x2;
	filter->s3.x1 = s3x1;
	filter->s3.x2 = s3x2;
	filter->s4.x1 = s4x1;
	filter->s4.x2 = s4x2;
	filter->s1.y1 = s1y1;
	filter->s1.y2 = s1y2;
	filter->s2.y1 = s2y1;
	filter->s2.y2 = s2y2;
	filter->s3.y1 = s3y1;
	filter->s3.y2 = s3y2;
	filter->s4.y1 = s4y1;
	filter->s4.y2 = s4y2;
}

void smfbs8(struct smf8o *filter, int n, float *y, float *x, float *f, float *Q) {
	int i;
	float _y, s1y1, s1y2, s2y1, s2y2, s3y1, s3y2, s4y1, s4y2,
	      _x, s1x1, s1x2, s2x1, s2x2, s3x1, s3x2, s4x1, s4x2, w;
	s1x1 = filter->s1.x1;
	s1x2 = filter->s1.x2;
	s2x1 = filter->s2.x1;
	s2x2 = filter->s2.x2;
	s3x1 = filter->s3.x1;
	s3x2 = filter->s3.x2;
	s4x1 = filter->s4.x1;
	s4x2 = filter->s4.x2;
	s1y1 = filter->s1.y1;
	s1y2 = filter->s1.y2;
	s2y1 = filter->s2.y1;
	s2y2 = filter->s2.y2;
	s3y1 = filter->s3.y1;
	s3y2 = filter->s3.y2;
	s4y1 = filter->s4.y1;
	s4y2 = filter->s4.y2;
	for (i = 0; i < n; i++) {
		_x = x[i];
		w = smff2fw(f[i]);
		_y = smf2s(_x, s1x1, s1x2, s1y1, s1y2, w, SMF_BUTTERWORTH_P24);
		s1y2 = s1y1;
		s1y1 = _y;
		s1x2 = s1x1;
		s1x1 = _x;
		_x = _y;
		_y = smf2s(_x, s2x1, s2x2, s2y1, s2y2, w, SMF_BUTTERWORTH_P24_INV);
		s2y2 = s2y1;
		s2y1 = _y;
		s2x2 = s2x1;
		s2x1 = _x;
		_x = _y;
		_y = smf2s(_x, s3x1, s3x2, s3y1, s3y2, w, SMF_BUTTERWORTH_P48);
		s3y2 = s3y1;
		s3y1 = _y;
		s3x2 = s3x1;
		s3x1 = _x;
		_x = _y;
		_y = smf2s(_x, s4x1, s4x2, s4y1, s4y2, w, SMF_BUTTERWORTH_P48_INV / Q[i]);
		s3y2 = s3y1;
		s3y1 = _y;
		s3x2 = s3x1;
		s3x1 = _x;
		y[i] = _y;
	}
	filter->s1.x1 = s1x1;
	filter->s1.x2 = s1x2;
	filter->s2.x1 = s2x1;
	filter->s2.x2 = s2x2;
	filter->s3.x1 = s3x1;
	filter->s3.x2 = s3x2;
	filter->s4.x1 = s4x1;
	filter->s4.x2 = s4x2;
	filter->s1.y1 = s1y1;
	filter->s1.y2 = s1y2;
	filter->s2.y1 = s2y1;
	filter->s2.y2 = s2y2;
	filter->s3.y1 = s3y1;
	filter->s3.y2 = s3y2;
	filter->s4.y1 = s4y1;
	filter->s4.y2 = s4y2;
}

