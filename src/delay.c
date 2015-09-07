/*
 * delay.c
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

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sonicmaths/delay.h"

int smdelay_init(struct smdelay *delay, int len) {
	delay->len = len;
	delay->i = 0;
	delay->x = malloc(sizeof(float) * len);
	if (delay->x == NULL) {
		return -1;
	}
	memset(delay->x, 0, sizeof(float) * len);

	return 0;
}

void smdelay_destroy(struct smdelay *delay) {
	free(delay->x);
}

void smtapdelay(struct smdelay *delay, int n, int ntaps, float **y, float *x,
		float **t) {
	int i, j, di, dlen, ti;
	float tf, tn, _y;
	di = delay->i;
	dlen = delay->len;
	for (i = 0; i < n; i++) {
		delay->x[di] = x[i];
		for (j = 0; j < ntaps; j++) {
			tf = t[j][i];
			tn = ceilf(tf);
			tf = tn - tf;
			ti = ((di + dlen) - (int) tn) % dlen;
			_y = delay->x[ti];
			_y += tf * (delay->x[(ti + 1) % dlen] - _y);
			y[j][i] = _y;
		}
		di = (di + 1) % dlen;
	}
	delay->i = di;
}

void smdelay(struct smdelay *delay, int n, float *y, float *x, float *t) {
	int i, di, dlen, ti;
	float tf, tn, _y;
	di = delay->i;
	dlen = delay->len;
	for (i = 0; i < n; i++) {
		delay->x[di] = x[i];
		tf = t[i];
		tn = ceilf(tf);
		tf = tn - tf;
		ti = ((di + dlen) - (int) tn) % dlen;
		_y = delay->x[ti];
		_y += tf * (delay->x[(ti + 1) % dlen] - _y);
		y[i] = _y;
		di = (di + 1) % dlen;
	}
	delay->i = di;
}
