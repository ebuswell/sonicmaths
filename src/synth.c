/*
 * synth.c
 * 
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
#include <atomickit/malloc.h>
#include <atomickit/rcp.h>
#include "sonicmaths/synth.h"

int smsynth_init(struct smsynth *synth, void (*destroy)(struct smsynth *)) {
	synth->t = amalloc(sizeof(float));
	if(synth->t == NULL) {
		return -1;
	}
	synth->t[0] = frandomf();
	synth->nchannels = 1;

	arcp_region_init(synth, (arcp_destroy_f) destroy);

	return 0;
}

void smsynth_destroy(struct smsynth *synth) {
	afree(synth->t, sizeof(float) * synth->nchannels);
}

static void __smsynth_destroy(struct smsynth *synth) {
	smsynth_destroy(synth);
	afree(synth, sizeof(struct smsynth));
}

struct smsynth *smsynth_create() {
	int r;
	struct smsynth *ret;

	ret = amalloc(sizeof(struct smsynth));
	if(ret == NULL) {
		return NULL;
	}

	r = smsynth_init(ret, __smsynth_destroy);
	if(r != 0) {
		afree(ret, sizeof(struct smsynth));
		return NULL;
	}

	return ret;
}
