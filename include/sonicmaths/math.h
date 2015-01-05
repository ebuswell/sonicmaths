/** @file math.h
 *
 * Miscellaneous mathematical functions for Sonic Maths.
 *
 */
/*
 * Copyright 2011 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, version 2.
 * 
 * Sonic Maths is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Sonic Maths.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef SONICMATHS_MATH_H
#define SONICMATHS_MATH_H 1

#include <stdlib.h>
#include <string.h>

#define frandomf() ((float) random() / (((float) RAND_MAX) + 1.0f))
#define randomf() ((double) random() / (((double) RAND_MAX) + 1.0))

static inline float smaths_normtime(float sample_rate, float t) {
	return t * sample_rate;
}

static inline float smaths_normfreq(float sample_rate, float f) {
	return f / sample_rate;
}

static void hadamard(int N, float matrix[N][N]) {
	if(N == 1) {
		matrix[0][0] = 1;
	} else {
		int N_2 = N >> 1;
		float submatrix[N_2][N_2];
		int i, j;
		float k;
		hadamard(N_2, submatrix);
		for(i = 0; i < N_2; i++) {
			for(j = 0; j < N_2; j++) {
				k = submatrix[i][j];
				matrix[i][j] = k;
				matrix[i + N_2][j] = k;
				matrix[i][j + N_2] = k;
				matrix[i + N_2][j + N_2] = -k;
			}
		}
	}
}

static inline void matvecmul(int N, float matrix[N][N], float vector[N]) {
	int i, j;
	float ret[N];
	memset(ret, 0, sizeof(float) * N);
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			ret[i] += matrix[i][j] * vector[j];
		}
	}
	memcpy(vector, ret, sizeof(float) * N);
}

#endif /* ! SONICMATHS_MATH_H */
