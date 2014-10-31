/*
 * envelope_generator.c
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

#include <atomickit/malloc.h>
#include <atomickit/rcp.h>
#include "sonicmaths/envelope-generator.h"

int smenvg_init(struct smenvg *envg, void (*destroy)(struct smenvg *)) {
	envg->state = amalloc(sizeof(struct smenvg_state));
	if(envg->state == NULL) {
		return -1;
	}
	memset(envg->state, 0, sizeof(struct smenvg_state));
	envg->nchannels = 1;

	arcp_region_init(envg, (arcp_destroy_f) destroy);

	return 0;
}

void smenvg_destroy(struct smenvg *envg) {
	afree(envg->state, sizeof(struct smenvg_state) * envg->nchannels);
}

static void __smenvg_destroy(struct smenvg *envg) {
	smenvg_destroy(envg);
	afree(envg, sizeof(struct smenvg));
}

struct smenvg *smenvg_create() {
	int r;
	struct smenvg *ret;

	ret = amalloc(sizeof(struct smenvg));
	if(ret == NULL) {
		return NULL;
	}

	r = smenvg_init(ret, __smenvg_destroy);
	if(r != 0) {
		afree(ret, sizeof(struct smenvg));
		return NULL;
	}

	return ret;
}
