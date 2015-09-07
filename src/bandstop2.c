/*
 * second-order.c
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
#include "sonicmaths/math.h"
#include "sonicmaths/second-order.h"
#include "sonicmaths/bandstop2.h"

void smbandstop2(struct sm2order *filter,
		 int n, float *y, float *x, float *f, float *Q) {
	int i;
	float _y, y1, y2, _x, x1, x2;
	x1 = filter->x1;
	x2 = filter->x2;
	y1 = filter->y1;
	y2 = filter->y2;
	for (i = 0; i < n; i++) {
		_x = x[i];
		_y = smbandstop2v(y1, y2, _x, x1, x2, f[i], Q[i]);
		y2 = y1;
		y1 = _y;
		x2 = x1;
		x1 = _x;
		y[i] = _y;
	}
	filter->y1 = SMFPNORM(y1);
	filter->y2 = SMFPNORM(y2);
	filter->x1 = x1;
	filter->x2 = x2;
}
