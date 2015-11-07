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

#include <sonicmaths/filter.h>

struct smfdmod {
	float *u;
	int maxnbanks;
};

int smfdmod_init(struct smfdmod *mod, int maxnbanks);
void smfdmod_destroy(struct smfdmod *mod);

void smfdmod(struct smfdmod *mod, int n, float *y, float *a, float *b,
	     float *bw);

#endif /* ! SONICMATHS_FDMODULATOR_H */
