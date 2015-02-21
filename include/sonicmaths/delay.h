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
	float f, n, r;
	struct {
		float t0;
		float t;
		float f;
		float n;
		float r;
		size_t i;
	} debug;
	size_t i;
	size_t len;

	debug.t0 = t;

	/* setup */
	i = delay->i;
	len = delay->len;

	/* calculate delay */
	n = fminf(SMDELAY_WSINCN, 2.0f * floorf(t - 0.5f) + 1.0f);
	if (n < 3.0f) {
		/* linear interpolation */
		f = t;
		t = ceilf(t);
		f = t - f;
		i = ((i + len) - (size_t) t) % len;
		r = delay->x[i++];
		return r + f * (delay->x[i % len] - r);
	} else {
		/* windowed sinc interpolation */
		t -= n / 2.0f;
		f = t;
		t = ceilf(t);
		f = t - f;
		i = ((i + len) - (size_t) (t + n)) % len;
		debug.t = t;
		debug.f = f;
		debug.n = n;
		debug.i = i;
		r = 0.0f;
		for (; f <= n; f += 1.0f, i = (i + 1) % len) {
			r += delay->x[i] * smaths_wsinc(f, n);
		}
		if (delay->i == 1) {
			post("t0: %f, t: %f, f: %f, n: %f, i: %zd, r: %f\n",
			     debug.t0, debug.t, debug.f, debug.n, debug.i, r);
		}
	}
	return r;
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
