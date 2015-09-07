/** @file sample-and-hold.h
 *
 * Sample and hold filter
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
#ifndef SONICMATHS_SAMPLE_AND_HOLD_H
#define SONICMATHS_SAMPLE_AND_HOLD_H 1

enum smsandh_ctl {
	SMSANDH_OFF,
	SMSANDH_ON
};

struct smsandh {
	float x;
	enum smsandh_ctl ctl;
};

/**
 * Destroy sample and hold
 */
void smsandh_destroy(struct smsandh *sandh);

/**
 * Initialize sample and hold
 */
int smsandh_init(struct smsandh *sandh);

void smsandh(struct smsandh *sandh, int n, float *y, float *x, float *ctl);

#endif
