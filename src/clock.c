/*
 * clock.c
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

#include <atomickit/rcp.h>
#include <atomickit/malloc.h>
#include "sonicmaths/clock.h"

int smclock_init(struct smclock *clock,
		void (*destroy)(struct smclock *clock)) {
	clock->t = amalloc(sizeof(float));
	if(clock->t == NULL) {
		return -1;
	}
	clock->t[0] = 0.0f;

	arcp_region_init(clock, (arcp_destroy_f) destroy);
	return 0;
}

void smclock_destroy(struct smclock *clock) {
	afree(clock->t, sizeof(float) * clock->nchannels);
}

static void __smclock_destroy(struct smclock *clock) {
	smclock_destroy(clock);
	afree(clock, sizeof(struct smclock));
}

struct smclock *smclock_create() {
	int r;
	struct smclock *clock;

	clock = amalloc(sizeof(struct smclock));
	if(clock == NULL) {
		return NULL;
	}

	r = smclock_init(clock, __smclock_destroy);
	if(r != 0) {
		afree(clock, sizeof(struct smclock));
		return NULL;
	}

	return clock;
}

