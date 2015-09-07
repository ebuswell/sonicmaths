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
#include "sonicmaths/lowpass2.h"
#include "sonicmaths/highpass2.h"
#include "sonicmaths/fdmodulator.h"

int smfdmod_init(struct smfdmod *mod, int maxnbanks) {
	mod->highf = calloc(maxnbanks - 1, sizeof(struct sm2order) * 4);
	if (mod->highf == NULL) {
		return -1;
	}
	mod->lowf = calloc(maxnbanks - 1, sizeof(struct sm2order) * 4);
	if (mod->lowf == NULL) {
		free(mod->highf);
		return -1;
	}
	/* adjust this so that we can use consistent indices for both
	 * arrays */
	mod->highf -= sizeof(struct sm2order) * 4;
	mod->maxnbanks = maxnbanks;
	return 0;
}

void smfdmod_destroy(struct smfdmod *mod) {
	free(mod->lowf);
	free(mod->highf + sizeof(struct sm2order) * 4);
}

static float smfdmod_lr_lpv(struct sm2order *filter, float f, float x) {
	float r;
	r = smlowpass2v(filter[0].y1, filter[0].y2,
			x, filter[0].x1, filter[0].x2,
			f, SM2O_BUTTERWORTH_Q);
	filter[0].y2 = filter[0].y1;
	filter[0].y1 = r;
	filter[0].x2 = filter[0].x1;
	filter[0].x1 = x;
	x = r;
	r = smlowpass2v(filter[1].y1, filter[1].y2,
			x, filter[1].x1, filter[1].x2,
			f, SM2O_BUTTERWORTH_Q);
	filter[1].y2 = filter[1].y1;
	filter[1].y1 = r;
	filter[1].x2 = filter[1].x1;
	filter[1].x1 = x;
	return r;
}

static float smfdmod_lr_hpv(struct sm2order *filter, float f, float x) {
	float r;
	r = smhighpass2v(filter[0].y1, filter[0].y2,
			 x, filter[0].x1, filter[0].x2,
			 f, SM2O_BUTTERWORTH_Q);
	filter[0].y2 = filter[0].y1;
	filter[0].y1 = r;
	filter[0].x2 = filter[0].x1;
	filter[0].x1 = x;
	x = r;
	r = smhighpass2v(filter[1].y1, filter[1].y2,
			 x, filter[1].x1, filter[1].x2,
			 f, SM2O_BUTTERWORTH_Q);
	filter[1].y2 = filter[1].y1;
	filter[1].y1 = r;
	filter[1].x2 = filter[1].x1;
	filter[1].x1 = x;
	return r;
}

void smfdmod(struct smfdmod *mod, int n, float *y, float *a, float *b,
	     float *bankwidth) {
	float bankf, bankdelta, _y, a0, b0;
	int i, j;
	for (i = 0; i < n; i++) {
		bankf = bankdelta = bankwidth[i];
		a0 = a[i];
		b0 = b[i];
		_y = smfdmod_lr_lpv(mod->lowf[0][0], bankf, a0)
		     * smfdmod_lr_lpv(mod->lowf[0][1], bankf, b0);
		for (j = 1;
		     bankf + bankdelta < 0.5f;
		     j++, bankf += bankdelta) {
			_y += (smfdmod_lr_hpv(mod->lowf[j][0],
					      bankf,
			       smfdmod_lr_lpv(mod->highf[j][0],
					      bankf + bankdelta, a0)))
			      * (smfdmod_lr_hpv(mod->lowf[j][1],
					        bankf,
			         smfdmod_lr_lpv(mod->highf[j][1],
					        bankf + bankdelta, b0)));
		}
		_y += smfdmod_lr_hpv(mod->highf[j][0], bankf, a0)
		      * smfdmod_lr_hpv(mod->highf[j][1], bankf, b0);
		y[i] = _y;
	}
	mod->lowf[0][0][0].y1 = SMFPNORM(mod->lowf[0][0][0].y1);
	mod->lowf[0][0][0].y2 = SMFPNORM(mod->lowf[0][0][0].y2);
	mod->lowf[0][0][1].y1 = SMFPNORM(mod->lowf[0][0][1].y1);
	mod->lowf[0][0][1].y2 = SMFPNORM(mod->lowf[0][0][1].y2);
	mod->lowf[0][1][0].y1 = SMFPNORM(mod->lowf[0][1][0].y1);
	mod->lowf[0][1][0].y2 = SMFPNORM(mod->lowf[0][1][0].y2);
	mod->lowf[0][1][1].y1 = SMFPNORM(mod->lowf[0][1][1].y1);
	mod->lowf[0][1][1].y2 = SMFPNORM(mod->lowf[0][1][1].y2);
	for (i = 1; i < mod->maxnbanks - 1; i++) {
		mod->lowf[i][0][0].y1 = SMFPNORM(mod->lowf[i][0][0].y1);
		mod->lowf[i][0][0].y2 = SMFPNORM(mod->lowf[i][0][0].y2);
		mod->lowf[i][0][1].y1 = SMFPNORM(mod->lowf[i][0][1].y1);
		mod->lowf[i][0][1].y2 = SMFPNORM(mod->lowf[i][0][1].y2);
		mod->lowf[i][1][0].y1 = SMFPNORM(mod->lowf[i][1][0].y1);
		mod->lowf[i][1][0].y2 = SMFPNORM(mod->lowf[i][1][0].y2);
		mod->lowf[i][1][1].y1 = SMFPNORM(mod->lowf[i][1][1].y1);
		mod->lowf[i][1][1].y2 = SMFPNORM(mod->lowf[i][1][1].y2);
		mod->highf[i][0][0].y1 = SMFPNORM(mod->highf[i][0][0].y1);
		mod->highf[i][0][0].y2 = SMFPNORM(mod->highf[i][0][0].y2);
		mod->highf[i][0][1].y1 = SMFPNORM(mod->highf[i][0][1].y1);
		mod->highf[i][0][1].y2 = SMFPNORM(mod->highf[i][0][1].y2);
		mod->highf[i][1][0].y1 = SMFPNORM(mod->highf[i][1][0].y1);
		mod->highf[i][1][0].y2 = SMFPNORM(mod->highf[i][1][0].y2);
		mod->highf[i][1][1].y1 = SMFPNORM(mod->highf[i][1][1].y1);
		mod->highf[i][1][1].y2 = SMFPNORM(mod->highf[i][1][1].y2);
	}
	mod->highf[i][0][0].y1 = SMFPNORM(mod->highf[i][0][0].y1);
	mod->highf[i][0][0].y2 = SMFPNORM(mod->highf[i][0][0].y2);
	mod->highf[i][0][1].y1 = SMFPNORM(mod->highf[i][0][1].y1);
	mod->highf[i][0][1].y2 = SMFPNORM(mod->highf[i][0][1].y2);
	mod->highf[i][1][0].y1 = SMFPNORM(mod->highf[i][1][0].y1);
	mod->highf[i][1][0].y2 = SMFPNORM(mod->highf[i][1][0].y2);
	mod->highf[i][1][1].y1 = SMFPNORM(mod->highf[i][1][1].y1);
	mod->highf[i][1][1].y2 = SMFPNORM(mod->highf[i][1][1].y2);
}

