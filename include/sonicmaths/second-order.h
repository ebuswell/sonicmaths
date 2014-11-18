/** @file lowpass.h
 *
 * Second-order filter
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
#ifndef SONICMATHS_SECOND_ORDER_H
#define SONICMATHS_SECOND_ORDER_H 1

#include <atomickit/rcp.h>

struct sm2order_matrix {
	float x1; /** Previous input */
	float x2; /** Previous previous input */
	float y1; /** Previous output */
	float y2; /** Previous previous output */
};

/**
 * Second-order filter
 */
struct sm2order {
	struct arcp_region;
	int nchannels;
	struct sm2order_matrix *matrix;
};

/**
 * Destroy second-order filter
 */
void sm2order_destroy(struct sm2order *filter);

/**
 * Initialize second-order filter
 */
int sm2order_init(struct sm2order *filter,
                  void (*destroy)(struct sm2order *));

/**
 * Create second-order filter
 */
struct sm2order *sm2order_create(void);

static inline int sm2order_redim(struct sm2order *filter,
                                 int nchannels) {
	if(filter->nchannels != nchannels) {
		struct sm2order_matrix *matrix;
		matrix = arealloc(filter->matrix,
		                  sizeof(struct sm2order_matrix)
		                   * filter->nchannels,
		                  sizeof(struct sm2order_matrix)
		                   * nchannels);
		if(matrix == NULL) {
			return -1;
		}
		if(nchannels > filter->nchannels) {
			memset(matrix + filter->nchannels, 0,
			       sizeof(struct sm2order_matrix)
			        * (nchannels - filter->nchannels));
		}
		filter->matrix = matrix;
		filter->nchannels = nchannels;
	}
	return 0;
}

#endif /* ! SONICMATHS_SECOND_ORDER_H */
