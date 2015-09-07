/** @file clock.h
 * Clock
 *
 * The clock outputs a timestamp for each sample.
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
#ifndef SONICMATHS_CLOCK_H
#define SONICMATHS_CLOCK_H 1

/**
 * Clock
 *
 * The clock outputs a timestamp for each sample.
 */
struct smclock {
	double t; /** The current time */
};

/**
 * Initialize clock
 */
int smclock_init(struct smclock *clock);

/**
 * Destroy clock
 */
void smclock_destroy(struct smclock *clock);

static inline void smclock_set_time(struct smclock *clock, float time) {
	clock->t = (double) time;
}

static inline float smclock_get_time(struct smclock *clock) {
	return (float) clock->t;
}

/**
 * Get the current time for a given rate.
 */
void smclock(struct smclock *clock, int n, float *y, float *rate);

#endif /* ! SONICMATHS_CLOCK_H */
