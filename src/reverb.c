/*
 * reverb.c
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

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sonicmaths/reverb.h"
#include "sonicmaths/random.h"

int smverb_init(struct smverb *verb,
                float echo_t, float echo_dev, size_t ntanks) {
	size_t tanklen;
	ssize_t i;
	float u, s;

	echo_t *= echo_t;
	echo_dev *= echo_dev;

	u = logf(echo_t / sqrtf(echo_dev + echo_t));
	s = sqrt(logf(1 + echo_dev / echo_t));

	verb->tanks = malloc(sizeof(struct smverb_tank *) * ntanks);
	if (verb->tanks == NULL) {
		return -1;
	}

	for (i = 0; i < (ssize_t) ntanks; i++) {
		echo_t = 2 * expf(u + s * smrand_gaussian());
		tanklen = (size_t) ceilf(echo_t);
		verb->tanks[i] = malloc(sizeof(struct smverb_tank)
		                        + sizeof(float) * tanklen);
		if (verb->tanks[i] == NULL) {
			goto error;
		}
		memset(verb->tanks[i]->tank, 0, sizeof(float) * tanklen);
		verb->tanks[i]->len = tanklen;
		verb->tanks[i]->f = echo_t - floorf(echo_t);
		verb->tanks[i]->i = 0;
	}

	verb->ntanks = ntanks;

	return 0;

error:
	for (i--; i >= 0; i--) {
		free(verb->tanks[i]);
	}
	free(verb->tanks);

	return -1;
}

void smverb_destroy(struct smverb *verb) {
	size_t i;
	for (i = 0; i < verb->ntanks; i++) {
		free(verb->tanks[i]);
	}
	free(verb->tanks);
}
