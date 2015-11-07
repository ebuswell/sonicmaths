/*
 * fdmodulator.c
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
#include "sonicmaths/math.h"
#include "sonicmaths/filter.h"
#include "sonicmaths/fdmodulator.h"

int smfdmod_init(struct smfdmod *mod, int maxnbanks) {
	mod->u = calloc(maxnbanks, sizeof(float) * 6 * 2);
	if (mod->u == NULL) {
		return -1;
	}
	return 0;
}

void smfdmod_destroy(struct smfdmod *mod) {
	free(mod->u);
}

void smfdmod(struct smfdmod *mod, int n, float *y, float *a, float *b,
	     float *bw) {
	int i, j;
	float f, _bw, alow, blow, ahigh, bhigh, _y;
	for (i = 0; i < n; i++) {
		_y = 0;
		_bw = bw[i];
		ahigh = a[i];
		bhigh = b[i];
		for (j = 0, f = _bw; f < 0.5f; f += _bw, j++) {
			smf4linkwitz_rileyv(mod->u+6*j, &alow, &ahigh, ahigh,
					    smff2w_2(f));
			smf4linkwitz_rileyv(mod->u+12*j, &blow, &bhigh, bhigh,
					    smff2w_2(f));
			_y += alow * blow;
		}
		_y += ahigh * bhigh;
		y[i] = _y;
	}
}
