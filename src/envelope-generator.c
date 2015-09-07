/*
 * envelope_generator.c
 * 
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

#include <string.h>
#include <math.h>
#include "sonicmaths/math.h"
#include "sonicmaths/envelope-generator.h"

#define CTL_THRESHOLD 0.2f

int smenvg_init(struct smenvg *envg) {
	memset(envg, 0, sizeof(struct smenvg));
	return 0;
}

void smenvg_destroy(struct smenvg *envg __attribute__((unused))) {
	/* Do nothing */
}

/**
 * Calculate the envelope value for a given target, previous value, start
 * value, and total time.
 */
static inline float smenvglv(float y1, float x, float xo, float T) {
	return y1 + copysignf(x - xo, x - y1) / T;
}

void smenvgl(struct smenvg *envg, int n, float *y, float *ctl,
	     float *attack_t, float *attack_a, float *decay_t,
	     float *sustain_a, float *release_t, float *release_a) {
	float _y, y1, x, xo, T;
	int i;
	y1 = envg->y1;

	i = 0;
	switch (envg->stage) {
	case ENVG_ATTACK:
	attack:
		do {
			x = attack_a[i];
			xo = release_a[i];
			T = attack_t[i];
			if (T <= 0) {
				y1 = x;
				goto decay;
			}
			_y = smenvglv(y1, x, xo, T);
			if ((_y <= x && x <= y1)
			    || (_y >= x && x >= y1)) {
				y1 = x;
				goto decay;
			}
			y1 = _y;
			y[i++] = _y;
		} while (i < n);
		envg->stage = ENVG_ATTACK;
		envg->y1 = SMFPNORM(y1);
		return;
	case ENVG_DECAY:
	decay:
		do {
			if (ctl[i] < CTL_THRESHOLD) {
				goto release;
			}
			x = sustain_a[i];
			xo = attack_a[i];
			T = decay_t[i];
			if (T <= 0) {
				y1 = x;
				goto sustain;
			}
			_y = smenvglv(y1, x, xo, T);
			if ((_y <= x && x <= y1)
			    || (_y >= x && x >= y1)) {
				y1 = x;
				goto sustain;
			}
			y1 = _y;
			y[i++] = _y;
		} while (i < n);
		envg->stage = ENVG_DECAY;
		envg->y1 = SMFPNORM(y1);
		return;
	case ENVG_SUSTAIN:
	sustain:
		do {
			if (ctl[i] < CTL_THRESHOLD) {
				goto release;
			}
			_y = sustain_a[i];
			y1 = _y;
			y[i++] = _y;
		} while (i < n);
		envg->stage = ENVG_SUSTAIN;
		envg->y1 = SMFPNORM(y1);
		return;
	case ENVG_RELEASE:
	release:
		do {
			if (ctl[i] > CTL_THRESHOLD) {
				goto attack;
			}
			x = release_a[i];
			xo = sustain_a[i];
			T = release_t[i];
			if (T <= 0) {
				y1 = x;
				goto finished;
			}
			_y = smenvglv(y1, x, xo, T);
			if ((_y <= x && x <= y1)
			    || (_y >= x && x >= y1)) {
				y1 = x;
				goto finished;
			}
			y1 = _y;
			y[i++] = _y;
		} while (i < n);
		envg->stage = ENVG_RELEASE;
		envg->y1 = SMFPNORM(y1);
		return;
	case ENVG_FINISHED:
	default:
	finished:
		do {
			if (ctl[i] > CTL_THRESHOLD) {
				goto attack;
			}
			_y = release_a[i];
			y1 = _y;
			y[i++] = _y;
		} while (i < n);
		envg->stage = ENVG_RELEASE;
		envg->y1 = SMFPNORM(y1);
		return;
	}
}

/* e^-pi/(1-e^-pi), the magic number to adjust the approach rate such that the
 * attack reaches its target in time. */
#define ATTACK_MAGIC_ADJ 0.045165705363684115f

static inline float smenvgv(float y1, float x, float T) {
	return x - expf(((float) -M_PI)/T) * (x - y1);
}

static inline float smenvg_attackv(float y1, float x, float xo, float T) {
	return smenvgv(y1, x + copysignf(x - xo, x - y1) * ATTACK_MAGIC_ADJ, T);
}

void smenvg(struct smenvg *envg, int n, float *y, float *ctl,
	    float *attack_t, float *attack_a, float *decay_t,
	    float *sustain_a, float *release_t, float *release_a) {
	float _y, y1, x, xo, T;
	int i;
	y1 = envg->y1;

	i = 0;
	switch (envg->stage) {
	case ENVG_ATTACK:
	attack:
		do {
			x = attack_a[i];
			xo = release_a[i];
			T = attack_t[i];
			if (T <= 0) {
				y1 = x;
				goto decay;
			}
			_y = smenvg_attackv(y1, x, xo, T);
			if ((_y <= x && x <= y1)
			    || (_y >= x && x >= y1)) {
				y1 = x;
				goto decay;
			}
			y1 = _y;
			y[i++] = _y;
		} while (i < n);
		envg->stage = ENVG_ATTACK;
		envg->y1 = SMFPNORM(y1);
		return;
	case ENVG_DECAY:
	decay:
		do {
			if (ctl[i] < CTL_THRESHOLD) {
				goto release;
			}
			x = sustain_a[i];
			T = decay_t[i];
			if (T <= 0) {
				y1 = x;
				goto sustain;
			}
			_y = smenvgv(y1, x, T);
			y1 = _y;
			y[i++] = _y;
		} while (i < n);
		envg->stage = ENVG_DECAY;
		envg->y1 = SMFPNORM(y1);
		return;
	case ENVG_SUSTAIN:
	sustain:
		do {
			if (ctl[i] < CTL_THRESHOLD) {
				goto release;
			}
			_y = sustain_a[i];
			y1 = _y;
			y[i++] = _y;
		} while (i < n);
		envg->stage = ENVG_SUSTAIN;
		envg->y1 = SMFPNORM(y1);
		return;
	case ENVG_RELEASE:
	release:
		do {
			if (ctl[i] > CTL_THRESHOLD) {
				goto attack;
			}
			x = release_a[i];
			T = release_t[i];
			if (T <= 0) {
				goto finished;
			}
			_y = smenvgv(y1, x, T);
			y1 = _y;
			y[i++] = _y;
		} while (i < n);
		envg->stage = ENVG_RELEASE;
		envg->y1 = SMFPNORM(y1);
		return;
	case ENVG_FINISHED:
	default:
	finished:
		do {
			if (ctl[i] > CTL_THRESHOLD) {
				goto attack;
			}
			_y = release_a[i];
			y1 = _y;
			y[i++] = _y;
		} while (i < n);
		envg->stage = ENVG_RELEASE;
		envg->y1 = SMFPNORM(y1);
		return;
	}
}
