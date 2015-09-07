/*
 * reverb.c
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
#include <math.h>
#include "sonicmaths/math.h"
#include "sonicmaths/random.h"
#include "sonicmaths/reverb.h"

int smverb_init(struct smverb *verb, int delaylen, int ndelays) {
	int i;
	verb->ndelays = ndelays;
	verb->delaylen = delaylen;
	verb->delays = malloc(sizeof(struct smverb_delay) * ndelays);
	if (verb->delays == NULL) {
		return -1;
	}
	verb->tdist = malloc(sizeof(float) * ndelays);
	if (verb->tdist == NULL) {
		free(verb->delays);
		return -1;
	}
	for (i = 0; i < ndelays; i++) {
		verb->delays[i].i = 0;
		verb->delays[i].x = calloc(delaylen, sizeof(float));
		if (verb->delays[i].x == NULL) {
			while (i--) {
				free(verb->delays[i].x);
			}
			free(verb->delays);
			free(verb->tdist);
			return -1;
		}
		verb->tdist[i] = smrand_gaussianv();
	}
	return 0;
}

void smverb_destroy(struct smverb *verb) {
	int i;
	for (i = 0; i < verb->ndelays; i++) {
		free(verb->delays[i].x);
	}
	free(verb->delays);
	free(verb->tdist);
}

void smverb(struct smverb *verb, int n, float *y, float *x, float *t,
	    float *tdev, float *g) {
	int i, j, N, xi, ti, dlen;
	float _y, yj, fb, tf, tn, fN;

	N = verb->ndelays;
	fN = (float) N;
	dlen = verb->delaylen;

	for (i = 0; i < n; i++) {
		_y = 0;
		for (j = 0; j < N; j++) {
			xi = verb->delays[j].i;
			tf = t[i] + tdev[i] * verb->tdist[j];
			tn = ceilf(tf);
			tf = tn - tf;
			ti = ((xi + dlen) - (int) tn) % dlen;
			yj = verb->delays[j].x[ti];
			yj += tf * (verb->delays[j].x[(ti + 1) % dlen] - yj);
			_y += yj;
			verb->delays[j].x[xi] = x[i] + g[i] * yj;
		}
		fb = -2 * g[i] * _y / fN;
		for (j = 0; j < N; j++) {
			xi = verb->delays[j].i;
			verb->delays[j].x[xi]
				= SMFPNORM(verb->delays[j].x[xi] + fb);
			verb->delays[j].i = (xi + 1) % dlen;
		}
		y[i] = _y;
	}
}

