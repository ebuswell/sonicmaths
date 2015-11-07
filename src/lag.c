/*
 * lag.c
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
#include "sonicmaths/lag.h"

int smlag_init(struct smlag *lag) {
	memset(lag, 0, sizeof(struct smlag));
	return 0;
}

void smlag_destroy(struct smlag *lag __attribute__((unused))) {
	/* Do nothing */
}

void smlag(struct smlag *lag, int n, float *y, float *x, float *t) {
	int i;
	float _y, y1, _x, x1, xo, T;
	y1 = lag->y1;
       	x1 = lag->x1;
	xo = lag->xo;
	for (i = 0; i < n; i++) {
		_x = x[i];
		T = t[i];
		if (_x != x1) {
			xo = x1;
		}
		_y = y1 + (_x - xo) / T;
		if ((xo <= _x && _y > _x)
		    || (_y < _x)) {
			_y = _x;
		}
		x1 = _x;
		y1 = _y;
		y[i] = _y;
	}
	lag->y1 = SMFPNORM(y1);
	lag->x1 = x1;
	lag->xo = xo;
}

void smlage(struct smlag *lag, int n, float *y, float *x, float *t) {
	int i;
	float _y, y1, _x, T;
	y1 = lag->y1;
	for (i = 0; i < n; i++) {
		_x = x[i];
		T = t[i];
		_y = _x - expf(((float) -M_PI)/T) * (_x - y1);
		if ((_y <= _x && _x <= y1)
		    || (_y >= _x && _x >= y1)) {
			_y = _x;
		}
		y1 = _y;
		y[i] = _y;
	}
	lag->y1 = SMFPNORM(y1);
}
