/** @file envelope_generator.h
 *
 * Envelope Generator
 *
 * Creates an envelope corresponding to a control signal.
 *
 * @verbatim

          ^ attack_a
        /  \
      /     \
    /        \_________ sustain_a
  /                    \
/                       \
release_a                release_a
|_________|___|      |___|
 attack_t  decay_t   release_t
@endverbatim
 *
 * If you want the addition of "hold" time, run the output through a
 * distortion filter. That's likely to give you a more releastic punch then
 * adding in a hold parameter anyway.
 *
 * Also, note that by setting the appropriate parameters you can invert the
 * envelope or do many other nonstandard things useful in controlling filters.
 *
 * The attack cycle is always performed.  A control signal to release during
 * this time will cause the release to begin immediately after the attack.
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
};

/**
 * Initialize envelope generator
 */
int smenvg_init(struct smenvg *envg);

/**
 * Destroy envelope generator
 */
void smenvg_destroy(struct smenvg *envg);

/**
 * Calculate the envelope from a set of input parameters.
 */
void smenvg(struct smenvg *envg, int n, float *y, float *ctl,
	    float *attack_t, float *attack_a, float *decay_t,
	    float *sustain_a, float *release_t, float *release_a);

void smenvgl(struct smenvg *envg, int n, float *y, float *ctl,
	     float *attack_t, float *attack_a, float *decay_t,
	     float *sustain_a, float *release_t, float *release_a);

#endif
