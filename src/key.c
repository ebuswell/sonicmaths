/*
 * key.c
 * 
 * Copyright 2014 Evan Buswell
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

#include <math.h>
#include "sonicmaths/key.h"

void smn2f(int n, float *f, float *note, float *root) {
	while (n--) {
		f[n] = smn2fv(note[n], root[n]);
	}
}

void smf2n(int n, float *note, float *f, float *root) {
	while (n--) {
		note[n] = smf2nv(f[n], root[n]);
	}
}

struct smkey_western smkey_equal = {
    13, {
	1.00000000000000000,
	1.05946309435929531,
	1.12246204830937302,
	1.18920711500272103,
	1.25992104989487319,
	1.33483985417003437,
	1.41421356237309515,
	1.49830707687668152,
	1.58740105196819936,
	1.68179283050742900,
	1.78179743628067855,
	1.88774862536338683,
	2.00000000000000000
    }
};

struct smkey_western smkey_harmonic = {
    13, {
	1.0,
	17.0/16.0,
	9.0/8.0,
	6.0/5.0,
	5.0/4.0,
	4.0/3.0,
	11.0/8.0,
	3.0/2.0,
	(4.0/3.0)*(6.0/5.0),
	(4.0/3.0)*(5.0/4.0),
	(3.0/2.0)*(6.0/5.0),
	(3.0/2.0)*(5.0/4.0),
	2.0
    }
};

struct smkey_western smkey_pythagorean = {
    13, {
	1.0,
	256.0/243.0,
	9.0/8.0,
	32.0/27.0,
	81.0/64.0,
	4.0/3.0,
	1024.0/729.0,
	32.0/27.0,
	128.0/81.0,
	27.0/16.0,
	16.0/9.0,
	243.0/128.0,
	2.0
    }
};

void smkey(struct smkey *key, int n, float *f, float *note, float *root) {
	int ni;
	float len, nn, nf, ne;

	len = (float) (key->len - 1);

	while (n--) {
		nf = note[n];
		ne = floorf(nf);
		nf = (nf - ne) * len;
		nn = floorf(nf);
		nf -= nn;
		ni = (int) nn;

		if (nf == 0) {
			f[n] = ldexpf(root[n] * key->tuning[ni], ne);
		} else {
			f[n] = ldexpf(root[n]
				      * key->tuning[ni]
				      * powf(key->tuning[ni + 1]
					     / key->tuning[ni], nf),
				      ne);
		}
	}
}

