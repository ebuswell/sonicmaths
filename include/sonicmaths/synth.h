/** @file synth.h
 *
 * Structure for generic synth functions
 *
 */
/*
 * Copyright 2013 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, version 2.
 * 
 * Sonic Maths is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * Sonic Maths.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SONICMATHS_SYNTH_H
#define SONICMATHS_SYNTH_H 1

#include <atomickit/rcp.h>
#include <atomickit/malloc.h>
#include <sonicmaths/math.h>

/**
 * Structure for generic synth functions
 */
struct smsynth {
	struct arcp_region;
	int nchannels; /**< Number of channels for which we're currently storing
	                *   state */
	float *t; /** Current time offset of the wave */
};

/**
 * Initialize synth
 */
int smsynth_init(struct smsynth *synth, void (*destroy)(struct smsynth *));

/**
 * Create synth
 */
struct smsynth *smsynth_create(void);

/**
 * Destroy synth
 */
void smsynth_destroy(struct smsynth *synth);

static inline void smsynth_set_phase(struct smsynth *synth,
                                     int channel, float phase) {
	synth->t[channel] = phase;
}

static inline float smsynth_get_phase(struct smsynth *synth,
                                      int channel) {
	return synth->t[channel];
}

/**
 * Redim the synth state based on number of channels.
 */
static inline int smsynth_redim(struct smsynth *synth, int nchannels) {
	if(nchannels != synth->nchannels) {
		int i;
		float *t;

		t = arealloc(synth->t, sizeof(float) * synth->nchannels,
                             sizeof(float) * nchannels);
		if(t == NULL) {
			return -1;
		}
		for(i = synth->nchannels; i < nchannels; i++) {
			t[i] = frandomf();
		}
		synth->nchannels = nchannels;
		synth->t = t;
	}
	return 0;
}

#endif /* ! SONICMATHS_SYNTH_H */
