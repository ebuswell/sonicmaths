/*
 * differentiator.c
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
#include "sonicmaths/math.h"
#include "sonicmaths/differentiator.h"

int smdiff_init(struct smdiff *diff) {
	memset(diff, 0, sizeof(struct smdiff));
	return 0;
}

void smdiff_destroy(struct smdiff *diff __attribute__((unused))) {
	/* Do nothing */
}

#define SMDIFF_WSINC_0 1.36729187f
#define SMDIFF_WSINC_1 -0.171034501f
#define SMDIFF_WSINC_2 0.0314555865f

void smdiff(struct smdiff *diff, int n, float *y, float *x) {
	int i;
	float _x, x1, x2, x3, x4, x5;
	x1 = diff->x1;
	x2 = diff->x2;
	x3 = diff->x3;
	x4 = diff->x4;
	x5 = diff->x5;
	for (i = 0; i < n; i++) {
		_x = x[i];
		y[i] =   (_x - x5) * SMDIFF_WSINC_2
		       + (x1 - x4) * SMDIFF_WSINC_1
		       + (x2 - x3) * SMDIFF_WSINC_0;
		x5 = x4;
		x3 = x2;
		x2 = x1;
		x1 = _x;
	}
	diff->x1 = x1;
	diff->x2 = x2;
	diff->x3 = x3;
	diff->x4 = x4;
	diff->x5 = x5;
}
