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

#include <math.h>
#include <sonicmaths/math.h>

#define SMDELAY_WSINCN 21.0f

struct smdelay {
	size_t len;
	size_t i;
	float *x;
};

/**
 * Destroy delay
 */
void smdelay_destroy(struct smdelay *delay);

/**
 * Initialize delay
 */
int smdelay_init(struct smdelay *delay, size_t len);

#include <m_pd.h>

static inline float smdelay_calc(struct smdelay *delay, float t) {
	float f, r;
	size_t i;
	size_t len;

	/* setup */
	i = delay->i;
	len = delay->len;

	/* linear interpolation */
	f = t;
	t = ceilf(t);
	f = t - f;
	i = ((i + len) - (size_t) t) % len;
	r = delay->x[i++];
	return r + f * (delay->x[i % len] - r);
}

static inline float smdelay(struct smdelay *delay, float x, float t) {
	size_t i;
	float y;
	i = delay->i;
	delay->x[i] = x;
	y = smdelay_calc(delay, t);
       	delay->i = (i + 1) % delay->len;
	return y;
}

#endif
