/*
 * sample-and-hold.c
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
#include "sonicmaths/sample-and-hold.h"

#define CTL_THRESHOLD 0.2f

int smsandh_init(struct smsandh *sandh) {
	sandh->x = 0.0f;
	sandh->ctl = SMSANDH_OFF;
	return 0;
}

void smsandh_destroy(struct smsandh *sandh __attribute__((unused))) {
	/* Do nothing */
}

void smsandh(struct smsandh *sandh, int n, float *y, float *x, float *ctl) {
	int i;
	float _x;
	_x = sandh->x;
	switch (sandh->ctl) {
	case SMSANDH_ON:
		for (i = 0; i < n; i++) {
			if (ctl[i] < CTL_THRESHOLD) {
				goto off;
			}
		on:
			y[i] = _x;
		}
		sandh->ctl = SMSANDH_ON;
		break;
	case SMSANDH_OFF:
	default:
		for (i = 0; i < n; i++) {
			if (ctl[i] > CTL_THRESHOLD) {
				_x = x[i];
				goto on;
			}
		off:
			y[i] = _x;
		}
		sandh->ctl = SMSANDH_OFF;
		break;
	}
	sandh->x = _x;
}
