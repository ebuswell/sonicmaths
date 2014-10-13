/*
 * integrator.c
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
#include <string.h>
#include <atomickit/malloc.h>
#include <atomickit/rcp.h>
#include "sonicmaths/integrator.h"

int smintg_init(struct smintg *intg, void (*destroy)(struct smintg *)) {
	intg->matrix = amalloc(sizeof(struct smintg_matrix));
	if(intg->matrix == NULL) {
		return -1;
	}
	memset(intg->matrix, 0, sizeof(struct smintg_matrix));
	intg->nchannels = 1;

	arcp_region_init(intg, (arcp_destroy_f) destroy);

	return 0;
}

void smintg_destroy(struct smintg *intg) {
	afree(intg->matrix, sizeof(struct smintg_matrix) * intg->nchannels);
}

static void __smintg_destroy(struct smintg *intg) {
	smintg_destroy(intg);
	afree(intg, sizeof(struct smintg));
}

struct smintg *smintg_create() {
	int r;
	struct smintg *ret;

	ret = amalloc(sizeof(struct smintg));
	if(ret == NULL) {
		return NULL;
	}

	r = smintg_init(ret, __smintg_destroy);
	if(r != 0) {
		afree(ret, sizeof(struct smintg));
	}

	return ret;
}

