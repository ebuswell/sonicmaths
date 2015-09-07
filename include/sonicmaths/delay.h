/** @file delay.h
 *
 * Delay filter.h
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
#ifndef SONICMATHS_DELAY_H
#define SONICMATHS_DELAY_H 1

struct smdelay {
	int len;
	int i;
	float *x;
};

/**
 * Destroy delay
 */
void smdelay_destroy(struct smdelay *delay);

/**
 * Initialize delay
 */
int smdelay_init(struct smdelay *delay, int len);

void smtapdelay(struct smdelay *delay, int n, int ntaps, float **y, float *x,
		float **t);

void smdelay(struct smdelay *delay, int n, float *y, float *x, float *t);

#endif
