/*
 * integrator.c
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
#include "sonicmaths/integrator.h"

int smintg_init(struct smintg *intg) {
	memset(intg, 0, sizeof(struct smintg));
	return 0;
}

void smintg_destroy(struct smintg *intg __attribute__((unused))) {
	/* Do nothing */
}

#define SMINTG_WSINC_0 0.851781806f
#define SMINTG_WSINC_1 0.0887156468f
#define SMINTG_WSINC_2 -0.0167572966f
#define SMINTG_WSINC_3 0.00215077831f

#define SMINTG_LEAKINESS 0.999f

void smintg(struct smintg *intg, int n, float *y, float *x) {
	int i;
	float _y, y1, _x, x1, x2, x3, x4, x5, x6;
	y1 = intg->y1;
	x1 = intg->x1;
	x2 = intg->x2;
	x3 = intg->x3;
	x4 = intg->x4;
	x5 = intg->x5;
	x6 = intg->x6;
	for (i = 0; i < n; i++) {
		_x = x[i];
		_y =   (_x + x6) * SMINTG_WSINC_3
		     + (x1 + x5) * SMINTG_WSINC_2
		     + (x2 + x4) * SMINTG_WSINC_1
		     + x3	 * SMINTG_WSINC_0
		     + y1	 * SMINTG_LEAKINESS;
		y1 = _y;
		x6 = x5;
		x5 = x4;
		x3 = x2;
		x2 = x1;
		x1 = _x;
		y[i] = _y;
	}
	intg->y1 = SMFPNORM(y1);
	intg->x1 = x1;
	intg->x2 = x2;
	intg->x3 = x3;
	intg->x4 = x4;
	intg->x5 = x5;
	intg->x6 = x6;
}
