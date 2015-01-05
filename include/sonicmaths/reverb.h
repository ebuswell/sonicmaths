/** @file reverb.h
 *
 * Reverb filter.h
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
#ifndef SONICMATHS_REVERB_H
#define SONICMATHS_REVERB_H 1

#include <string.h>
#include <math.h>
#include <float.h>
#include <atomickit/rcp.h>
#include <atomickit/malloc.h>
#include <sonicmaths/math.h>

struct smverb_tank {
	size_t i;
	size_t tanklen;
	float echo_t;
	float tank[];
};

struct smverb {
	struct arcp_region;
	size_t ntanks;
	struct smverb_tank **tanks;
};

/**
 * Destroy reverb
 */
void smverb_destroy(struct smverb *verb);

/**
 * Initialize reverb
 */
int smverb_init(struct smverb *verb,
                float echo_t, float echo_dev, size_t ntanks,
                void (*destroy)(struct smverb *));

/**
 * Create reverb
 */
struct smverb *smverb_create(float echo_t, float echo_dev, size_t ntanks);

// static inline int smverb_set_echo_t(struct smverb *verb, float t) {
// 	size_t newtanklen = (size_t) (2.0f * t);
// 
// 	if(newtanklen != verb->tanklen) {
// 		size_t len, i, rem;
// 		float *newtank;
//  		newtank	= amalloc(sizeof(float) * newtanklen);
// 		if(newtank == NULL) {
// 			return -1;
// 		}
// 		rem = newtanklen;
// 		i = 0;
// 		len = verb->tanklen - verb->yi;
// 		if(rem < len) {
// 			len = rem;
// 		}
// 		memcpy(&newtank[i], &verb->tank[verb->yi], sizeof(float) * len);
// 		i += len;
// 		rem -= len;
// 		len = verb->yi;
// 		if(rem < len) {
// 			len = rem;
// 		}
// 		memcpy(&newtank[i], &verb->tank[0], sizeof(float) * len);
// 		i += len;
// 		rem -= len;
// 		memset(&newtank[i], 0, sizeof(float) * rem);
// 		afree(verb->tank, sizeof(float) * verb->tanklen);
// 		verb->yi = newtanklen / 2;
// 		verb->tank = newtank;
// 		verb->tanklen = newtanklen;
// 	}
// 
// 	return 0;
// }

/* Transforms a number of tanks into the next power of two. Conceptually:
 * 2^(ceil(log2(size))) */
static inline int smverb_Nto2M(size_t size) {
	int r;
	size--;
	r = 0;
	while(size) {
		size >>= 1;
		r++;
	}
	return 1 << r;
}

static inline float smverb(struct smverb *verb, float x, float absorbtion) {
	int N = smverb_Nto2M(verb->ntanks);
	float fbmat[N][N];
	float yvec[N];

	float ret, y;
	struct smverb_tank *tank;
	size_t i;

	ret = 0.0f;

	absorbtion /= sqrtf(verb->ntanks);

	// calculate ret and set vector
	for(i = 0; i < verb->ntanks; i++) {
		tank = verb->tanks[i];
		y = tank->tank[tank->i];
		ret += y;
		yvec[i] = y;
	}
	ret /= (float) verb->ntanks;

	// round out vector with zeroes
	memset(&yvec[i], 0, sizeof(float) * (((size_t) N) - i));

	// calculate feedback vectors
	hadamard(N, fbmat);
	matvecmul(N, fbmat, yvec);

	// set feedback and increment delay pointer
	for(i = 0; i < verb->ntanks; i++) {
		tank = verb->tanks[i];
		y = absorbtion * yvec[i];
		if(isnormal(y) || y == 0) {
			/* y = y; */
		} else if(y == INFINITY) {
			y = FLT_MAX;
		} else if(y == -INFINITY) {
			y = -FLT_MAX;
		} else {
			y = 0;
		}
		tank->tank[tank->i] = x + y;
		tank->i++;
		tank->i %= tank->tanklen;
	}

	return ret;
}

#endif
