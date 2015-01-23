/** @file envelope_generator.h
 *
 * Envelope Generator
 *
 * Creates an envelope corresponding to a control signal.
 *
 * @verbatim
       __ attack_a
    _--  \
   -      \
  /         \________ sustain_a
 /                   \
/                     \
|                       \
release_a                release_a
|________|___|       |___|
 attack_t  decay_t    release_t
@endverbatim
 *
 * The bad ascii art is trying to illustrate the default exponential version.
 *
 * If you want the addition of "hold" time, run the output through a
 * distortion filter. That's likely to give you a more releastic punch then
 * adding in a hold parameter anyway.
 *
 * Also, note that by setting the appropriate parameters you can invert the
 * envelope or do many other nonstandard things useful in controlling filters.
 * The predictable way that a linear envelope interacts with lin2exp (@ref
 * lin2exp.h) is important for certain effects.
 *
 * The attack and decay cycle are always performed.  A control signal to
 * release during this time will cause the release to happen immediately after
 * the decay.  Conversely, a control signal to attack starts the attack cycle
 * immediately.
 *
 * As a last warning, I suspect the existence of bugs I have not yet been able
 * to identify when attack_a and release_a are not the default values.
 *
 * @todo parameterize a forced release.
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
#ifndef SONICMATHS_ENVELOPE_GENERATOR_H
#define SONICMATHS_ENVELOPE_GENERATOR_H 1

#include <stdbool.h>
#include <math.h>
#include <sonicmaths/math.h>

/**
 * Envelope Generator State
 */
enum smenvg_stage {
	ENVG_FINISHED = 0,
	ENVG_ATTACK,
	ENVG_DECAY,
	ENVG_SUSTAIN,
	ENVG_RELEASE
};

/**
 * Envelope Generator
 */
struct smenvg {
	enum smenvg_stage stage;
	float y1;
	bool release;
};

/**
 * Initialize envelope generator
 */
int smenvg_init(struct smenvg *envg);

/**
 * Initialize envelope generator
 */
void smenvg_destroy(struct smenvg *envg);

/* e^-pi, the base of the exponent for exponential decays. */
#define EXP_NEG_PI 0.043213918263772250f

/* e^-pi/(1-e^-pi), the magic number to adjust the approach rate such that the
 * attack reaches its target in time. */
#define ATTACK_MAGIC_ADJ 0.045165705363684115f

/**
 * Calculate the decay envelope value for a given target, previous value,
 * start vale, and total time.
 */
static inline float smenvg_decay_exp(float x,
				     float y0 __attribute__((unused)),
				     float y1, float T) {
	return x - powf(EXP_NEG_PI, 1/T) * (x - y1);
}

/**
 * Calculate the attack envelope value for a given target, previous value,
 * start value, and total time.
 */
static inline float smenvg_attack_exp(float x, float y0, float y1, float T) {
	x += copysignf(x - y0, x - y1) * ATTACK_MAGIC_ADJ;
	return smenvg_decay_exp(x, y0, y1, T);
}

/**
 * Calculate the envelope value for a given target, previous value, start
 * value, and total time.
 */
static inline float smenvg_stage_lin(float x, float y0, float y1, float T) {
	return y1 + copysignf(x - y0, x - y1) / T;
}

/**
 * Calculate the envelope value from a set of input parameters.
 */
static inline float smenvg(struct smenvg *envg, bool linear, float ctl,
			   float attack_t, float attack_a,
			   float decay_t, float sustain_a,
			   float release_t, float release_a) {
	float x;
	float y0;
	float y;
	float y1 = envg->y1;
	float T;
	if (ctl > 0) {
		/* attack event */
		envg->stage = ENVG_ATTACK;
		envg->release = false;
		goto start_attack;
	} else if (ctl < 0) {
		/* release event */
		envg->release = true;
	}

	switch (envg->stage) {
	case ENVG_ATTACK:
	start_attack:
		x = attack_a;
		y0 = release_a;
		T = attack_t;
		if (T == 0) {
			y1 = SMNORM(x);
			envg->stage = ENVG_DECAY;
			goto start_decay;
		}
		y = linear ? smenvg_stage_lin(x, y0, y1, T)
			   : smenvg_attack_exp(x, y0, y1, T);
		if ((y <= x && x <= y1)
		    || (y >= x && x >= y1)) {
			y = x;
			envg->stage = ENVG_DECAY;
		}
		break;
	case ENVG_DECAY:
	start_decay:
		x = sustain_a;
		y0 = attack_a;
		T = decay_t;
		if (T == 0) {
			envg->stage = ENVG_SUSTAIN;
			goto start_sustain;
		}
		y = linear ? smenvg_stage_lin(x, y0, y1, T)
			   : smenvg_decay_exp(x, y0, y1, T);
		if ((y <= x && x <= y1)
		    || (y >= x && x >= y1)) {
			envg->stage = ENVG_SUSTAIN;
			goto start_sustain;
		} else if (envg->release) {
			y1 = SMNORM(y);
			envg->stage = ENVG_RELEASE;
			goto start_release;
		}
		break;
	case ENVG_SUSTAIN:
	start_sustain:
		y = sustain_a;
		if (envg->release) {
			y1 = SMNORM(y);
			envg->stage = ENVG_RELEASE;
			goto start_release;
		}
		break;
	case ENVG_RELEASE:
	start_release:
		x = release_a;
		y0 = sustain_a;
		T = release_t;
		if (T == 0) {
			envg->stage = ENVG_FINISHED;
			goto start_finished;
		}
		y = linear ? smenvg_stage_lin(x, y0, y1, T)
			   : smenvg_decay_exp(x, y0, y1, T);
		if ((y <= x && x <= y1)
		    || (y >= x && x >= y1)) {
			y = x;
			envg->stage = ENVG_FINISHED;
		}
		break;
	case ENVG_FINISHED:
	start_finished:
	default:
		y = release_a;
	}

	envg->y1 = SMNORM(y);

	return y;
}

#endif
