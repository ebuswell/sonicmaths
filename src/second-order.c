/*
 * second-order.c
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
#include <string.h>
#include <atomickit/malloc.h>
#include <atomickit/rcp.h>
#include "sonicmaths/second-order.h"

int sm2order_init(struct sm2order *filter,
                  void (*destroy)(struct sm2order *)) {
	filter->matrix = amalloc(sizeof(struct sm2order_matrix));
	if(filter->matrix == NULL) {
		return -1;
	}
	memset(filter->matrix, 0, sizeof(struct sm2order_matrix));
	filter->nchannels = 1;

	arcp_region_init(filter, (arcp_destroy_f) destroy);

	return 0;
}

void sm2order_destroy(struct sm2order *filter) {
	afree(filter->matrix,
	      sizeof(struct sm2order_matrix) * filter->nchannels);
}

static void __sm2order_destroy(struct sm2order *filter) {
	sm2order_destroy(filter);
	afree(filter, sizeof(struct sm2order));
}

struct sm2order *sm2order_create() {
	int r;
	struct sm2order *ret;

	ret = amalloc(sizeof(struct sm2order));
	if(ret == NULL) {
		return NULL;
	}

	r = sm2order_init(ret, __sm2order_destroy);
	if(r != 0) {
		afree(ret, sizeof(struct sm2order));
	}

	return ret;
}
