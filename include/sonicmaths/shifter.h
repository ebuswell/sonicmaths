/** @file shifter.h
 *
 * Implements a frequency shifter.
 *
 */
/*
 * Copyright 2015 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 * 
 * Sonic Maths is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with Sonic Maths.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SONICMATHS_SHIFTER_H
#define SONICMATHS_SHIFTER_H 1

#include <sonicmaths/math.h>
#include <sonicmaths/oscillator.h>
#include <sonicmaths/second-order.h>

struct smhilbert_pair_sect {
	float x1;
	float x2;
	float y1;
	float y2;
};

struct smhilbert_pair {
	struct smhilbert_pair_sect sect[2][4];
	float y1;
};


/**
 * Frequency Shift Filter
 */
struct smshift {
	struct smhilbert_pair coeff;
	struct sm2order filter;
	struct smosc osc;
};

/**
 * Initialize frequency shift filter
 */
int smshift_init(struct smshift *shift);

/**
 * Destroy frequency shift filter
 */
void smshift_destroy(struct smshift *shift);

/**
 * Perform a frequency shift.
 */
void smshift(struct smshift *shift, int n, float *y, float *x, float *f);

#endif /* ! SONICMATHS_SHIFTER_H */
