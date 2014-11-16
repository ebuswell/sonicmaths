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
	float *t; /** The current time */
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

/**
 * Redim the clock state based on number of channels.
 */
static inline int smclock_redim(struct smclock *clock, int nchannels) {
	if(nchannels != clock->nchannels) {
		float *t = arealloc(clock->t,
		                    sizeof(float) * clock->nchannels,
		                    sizeof(float) * nchannels);
		if(t == NULL) {
	    		return -1;
		}
		if(clock->nchannels < nchannels) {
	    		memset(clock->t + clock->nchannels, 0,
			       sizeof(float)
			        * (nchannels - clock->nchannels));
		}
		clock->t = t;
		clock->nchannels = nchannels;
	}

	return 0;
}

/**
 * Get the current time for a given channel and rate.
 */
static inline float smclock(struct smclock *clock, int channel, float rate) {
	float ret = clock->t[channel];
	clock->t[channel] += rate;
	return ret;
}

#endif /* ! SONICMATHS_CLOCK_H */
