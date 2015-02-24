/** @file fdmodulator.h
 *
 * Frequency Domain Modulator
 *
 */
/*
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
#ifndef SONICMATHS_FDMODULATOR_H
#define SONICMATHS_FDMODULATOR_H 1

#include <math.h>
#include <sonicmaths/math.h>

struct smfdmod {
	size_t R;
	float *abuf[4];
	float *bbuf[2];
	fftwf_plan aplans[4];
	fftwf_plan bplans[2];
	fftwf_plan inv_plans[4];
	unsigned int cbuf;
	size_t i;
};

int smfdmod_init(struct smfdmod *mod, size_t len);
void smfdmod_destroy(struct smfdmod *mod);

static inline float smfdmod(struct smfdmod *mod, float a, float b) {
	size_t i = mod->i;
	size_t R = mod->R;
	unsigned int cbuf = mod->cbuf;
	float *ina1 = mod->abuf[cbuf];
	float *inb1 = mod->bbuf[cbuf % 2];
	float *ina2 = mod->abuf[(cbuf + 1) % 4];
	float *inb2 = mod->bbuf[(cbuf + 1) % 2];
	float *out1 = mod->abuf[(cbuf + 2) % 4];
	float *out2 = mod->abuf[(cbuf + 3) % 4];
	float ret;
	ret = out2[i] + out1[i + R];
	ina2[i] = ina1[i + R] = a;
	inb2[i] = inb1[i + R] = b;
	i = (i + 1) % R;
	mod->i = i;
	if (i == 0) {
		/* fun part */
		size_t n;
		float re, im;
		n = 2 * R;
		smstft(mod->aplans[cbuf], ina1, n);
		smstft(mod->bplans[cbuf % 2], inb1, n);
		/* Deal with the wholly real component */
		re = inb1[0];
		re = fabsf(re);
		ina1[0] *= re;
		for (i = 0; i < R; i++) {
			re = inb1[i];
			im = inb1[n - i];
			re = sqrtf(re * re + im * im);
			ina1[i] *= re;
			ina1[n - i] *= re;
		}
		/* Deal with remaining even component, if there is one. */
		if (!(n % 2)) {
			re = inb1[i];
			re = fabsf(re);
			ina1[i] *= re;
		}
		smstft_inv(mod->inv_plans[cbuf], ina1, n);
		mod->cbuf = (cbuf + 1) % 4;
	}
	return ret;
}

#endif /* ! SONICMATHS_FDMODULATOR_H */
