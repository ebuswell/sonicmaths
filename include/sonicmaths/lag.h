/** @file lag.h
 *
 * Lag Filter
 *
 * Causes instantanous changes to instead linearly progress from the old to
 * new value over a time lag.
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
#ifndef SONICMATHS_LAG_H
#define SONICMATHS_LAG_H 1

/**
 * Lag Filter
 */
struct smlag {
	float x1; /** The target value */
	float xo; /** The originating value */
	float y1; /** The previous value */
};

/**
 * Destroy lag filter
 */
void smlag_destroy(struct smlag *lag);

/**
 * Initialize lag filter
 */
int smlag_init(struct smlag *lag);

void smlag(struct smlag *lag, int n, float *y, float *x, float *t);

void smlage(struct smlag *lag, int n, float *y, float *x, float *t);

#endif
