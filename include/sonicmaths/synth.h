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
#ifndef SONICMATHS_SYNTH_H
#define SONICMATHS_SYNTH_H 1

#include <atomickit/atomic-malloc.h>
#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>

/**
 * Structure for generic synth functions
 */
struct smaths_synth {
    struct gln_node; /** Node for this synth */
    struct gln_socket *out; /** Output socket */
    struct smaths_parameter *freq; /** Frequency divided by sample rate */
    struct smaths_parameter *amp; /** Amplitude */
    struct smaths_parameter *phase; /** Offset of the cycle from zero */
    struct smaths_parameter *offset; /** Offset of the amplitude from zero */
    int nchannels; /** Number of channels for which we're currently storing state */
    float *t; /** Current time offset of the wave */
};

/**
 * Destroy synth
 */
void smaths_synth_destroy(struct smaths_synth *synth);

/**
 * Initialize synth
 */
int smaths_synth_init(struct smaths_synth *synth, struct smaths_graph *graph, gln_process_fp_t func, void (*destroy)(struct smaths_synth *));

static inline int smaths_synth_redim_state(struct smaths_synth *synth, int nchannels) {
    if(nchannels != synth->nchannels) {
	int i;
	float *t;

	t = arealloc(synth->t, sizeof(float) * synth->nchannels,
			    sizeof(float) * nchannels);
	if(t == NULL) {
	    return -1;
	}
	for(i = synth->nchannels; i < nchannels; i++) {
	    t[i] = t[i - 1];
	}
	synth->nchannels = nchannels;
	synth->t = t;
    }
    return 0;
}

#endif /* ! SONICMATHS_SYNTH_H */
