/** @file reverb.h
 *
 * Reverb filter.h
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
#ifndef SONICMATHS_REVERB_H
#define SONICMATHS_REVERB_H 1

struct smverb_delay {
	int i;
	float *x;
};

struct smverb {
	int ndelays;
	int delaylen;
	float *tdist;
	struct smverb_delay *delays;
};

int smverb_init(struct smverb *verb, int delaylen, int ndelays);

void smverb_destroy(struct smverb *verb);

void smverb(struct smverb *verb, int n, float *y, float *x, float *t,
	    float *tdev, float *g);

#endif
