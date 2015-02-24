/*
 * math.c
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
#include <fftw3.h>
#include "sonicmaths/math.h"

int smhilbert_pair_init(struct smhilbert_pair *coeff) {
	memset(coeff, 0, sizeof(struct smhilbert_pair));
	return 0;
}

void smhilbert_pair_destroy(struct smhilbert_pair *coeff __attribute__((unused))) {
	/* Do nothing */
}

fftwf_plan smstft_plan_create(float *x, size_t len) {
	return fftwf_plan_r2r_1d(len, x, x, FFTW_R2HC, FFTW_MEASURE);
}

fftwf_plan smstft_inv_plan_create(float *x, size_t len) {
	return fftwf_plan_r2r_1d(len, x, x, FFTW_HC2R, FFTW_MEASURE);
}

void smstft_plan_destroy(fftwf_plan plan) {
	fftwf_destroy_plan(plan);
}


