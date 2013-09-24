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

#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>
#include <sonicmaths/filter.h>

struct smaths_2order_matrix {
    float x1; /** Previous input */
    float x2; /** Previous previous input */
    float y1; /** Previous output */
    float y2; /** Previous previous output */
};

/**
 * Second-order filter
 *
 * See @ref struct smaths_filter
 */
struct smaths_2order {
    struct smaths_filter;
    struct smaths_parameter *freq; /** The corner frequency */
    struct smaths_parameter *Q; /** The filter's Q */
    int nchannels;
    struct smaths_2order_matrix *matrix;
};

/**
 * Destroy second-order filter
 *
 * See @ref smaths_filter_destroy
 */
void smaths_2order_destroy(struct smaths_2order *filter);

/**
 * Initialize second-order filter
 *
 * See @ref smaths_filter_init
 */
int smaths_2order_init(struct smaths_2order *filter, struct smaths_graph *graph, gln_process_fp_t func, void (*destroy)(struct smaths_2order *));

static inline int smaths_2order_redim_state(struct smaths_2order *filter, int nchannels) {
    if(filter->nchannels != nchannels) {
	struct smaths_2order_matrix *matrix;
	matrix = arealloc(filter->matrix, sizeof(struct smaths_2order_matrix) * filter->nchannels,
			  sizeof(struct smaths_2order_matrix) * nchannels);
	if(matrix == NULL) {
	    return -1;
	}
	if(nchannels > filter->nchannels) {
	    memset(matrix + filter->nchannels, 0, sizeof(struct smaths_2order_matrix) * (nchannels - filter->nchannels));
	}
	filter->matrix = matrix;
	filter->nchannels = nchannels;
    }
    return 0;
}

#endif /* ! SONICMATHS_SECOND_ORDER_H */
