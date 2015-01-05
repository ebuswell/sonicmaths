/*
 * reverb.c
 * 
 * Copyright 2014 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, version 2.
 * 
 * Sonic Maths is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Sonic Maths.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <math.h>
#include <atomickit/malloc.h>
#include <atomickit/rcp.h>
#include "sonicmaths/reverb.h"
#include "sonicmaths/noise.h"

int smverb_init(struct smverb *verb,
                float echo_t, float echo_dev, size_t ntanks,
                void (*destroy)(struct smverb *)) {
	size_t tanklen;
	ssize_t i;
	float u, s;

	echo_t *= echo_t;
	echo_dev *= echo_dev;

	u = logf(echo_t / sqrtf(echo_dev + echo_t));
	s = sqrt(logf(1 + echo_dev / echo_t));

	verb->tanks = amalloc(sizeof(struct smverb_tank *) * ntanks);
	if(verb->tanks == NULL) {
		return -1;
	}

	for(i = 0; i < (ssize_t) ntanks; i++) {
		tanklen = (size_t)
		          (2 * expf(u + s * smnoise(SMNOISE_GAUSSIAN)));
		verb->tanks[i] = amalloc(sizeof(struct smverb_tank)
		                         + sizeof(float) * tanklen);
		if(verb->tanks[i] == NULL) {
			goto error;
		}
		memset(verb->tanks[i]->tank, 0, sizeof(float) * tanklen);
		verb->tanks[i]->tanklen = tanklen;
		verb->tanks[i]->echo_t = tanklen;
		verb->tanks[i]->i = 0;
	}

	verb->ntanks = ntanks;

	arcp_region_init(verb, (arcp_destroy_f) destroy);

	return 0;

error:
	for(i--; i >= 0; i--) {
		afree(verb->tanks[i], sizeof(struct smverb_tank)
		                             + sizeof(float)
		                               * verb->tanks[i]->tanklen);
	}
	afree(verb->tanks, sizeof(struct smverb_tank *) * ntanks);

	return -1;
}

void smverb_destroy(struct smverb *verb) {
	size_t i;
	for(i = 0; i < verb->ntanks; i++) {
		afree(verb->tanks[i], sizeof(struct smverb_tank)
		                             + sizeof(float)
		                               * verb->tanks[i]->tanklen);
	}
	afree(verb->tanks, sizeof(struct smverb_tank *) * verb->ntanks);
}

static void __smverb_destroy(struct smverb *verb) {
	smverb_destroy(verb);
	afree(verb, sizeof(struct smverb));
}

struct smverb *smverb_create(float echo_t, float echo_dev, size_t ntanks) {
	int r;
	struct smverb *ret;

	ret = amalloc(sizeof(struct smverb));
	if(ret == NULL) {
		return NULL;
	}

	r = smverb_init(ret, echo_t, echo_dev, ntanks, __smverb_destroy);
	if(r != 0) {
		afree(ret, sizeof(struct smverb));
		return NULL;
	}

	return ret;
}

