/** @file clock.h
 * Clock
 *
 * The clock outputs a timestamp for each sample.
 */
/*
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
#ifndef SONICMATHS_CLOCK_H
#define SONICMATHS_CLOCK_H 1

#include <string.h>
#include <atomickit/rcp.h>

/**
 * Clock
 *
 * The clock outputs a timestamp for each sample.
 */
struct smclock {
	struct arcp_region;
	int nchannels; /** The number of channels of state we're currently
	                *  storing. */
	float t; /** The current time */
	float loop; /** Loop at this time */
};

/**
 * Initialize clock
 */
int smclock_init(struct smclock *clock,
                 void (*destroy)(struct smclock *clock));

/**
 * Create clock
 */
struct smclock *smclock_create(void);

/**
 * Destroy clock
 */
void smclock_destroy(struct smclock *clock);

static inline void smclock_set_loop(struct smclock *clock, float loop) {
	clock->loop = loop;
}

static inline float smclock_get_loop(struct smclock *clock) {
	return clock->loop;
}

static inline void smclock_set_time(struct smclock *clock, float time) {
	clock->t = time;
}

static inline float smclock_get_time(struct smclock *clock) {
	return clock->t;
}

/**
 * Get the current time for a given channel and rate.
 */
static inline float smclock(struct smclock *clock, float rate) {
	float ret = clock->t;
	clock->t += rate;
	while(clock->t > clock->loop) {
		clock->t -= clock->loop;
	}
	return ret;
}

#endif /* ! SONICMATHS_CLOCK_H */
