/** @file integrator.h
 *
 * Implements a simple integrator.
 *
 * This takes the integral as if the sampled point output was reconstructed
 * through a windowed sinc function (Blackman seems to work best) 9 samples
 * wide---therefore there's a 4 1/2 sample delay. This is then integrated and
 * sampled with no filtering (theoretically unnecessary).  The integrator is
 * "leaky," so that DC should not be introduced.
 *
 * @verbatim
y = y1 * LEAKINESS + (x + x8) * WSINC_4 + (x1 + x7) * WSINC_3
    + (x2 + x6) * WSINC_2 + (x3 + x5) * WSINC_1 + x4 * WSINC_0
@endverbatim
 *
 */
/*
 * Copyright 2013 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 * 
 * Sonic Maths is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * Sonic Maths.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SONICMATHS_INTEGRATOR_H
#define SONICMATHS_INTEGRATOR_H 1

#include <math.h>
#include <float.h>
#include <string.h>
#include <atomickit/rcp.h>

/**
 * Integration filter matrix
 */
struct smintg_matrix {
	float y1;
	float x1;
	float x2;
	float x3;
	float x4;
	float x5;
	float x6;
	float x7;
	float x8;
};

/**
 * Integration filter
 */
struct smintg {
	struct arcp_region;
	int nchannels;
	struct smintg_matrix *matrix;
};

/**
 * Initialize integration filter
 */
int smintg_init(struct smintg *intg, void (*destroy)(struct smintg *));

/**
 * Destroy integration filter
 */
void smintg_destroy(struct smintg *intg);

/**
 * Create integration filter
 */
struct smintg *smintg_create(void);

/**
 * Redim the integrator state based on number of channels.
 */
static inline int smintg_redim(struct smintg *intg, int nchannels) {
	if(nchannels != intg->nchannels) {
		struct smintg_matrix *matrix;

		matrix = arealloc(intg->matrix,
		                  sizeof(struct smintg_matrix)
		                   * intg->nchannels,
		                  sizeof(struct smintg_matrix)
		                   * nchannels);
		if(matrix == NULL) {
			return -1;
		}
		if(nchannels > intg->nchannels) {
			memset(matrix + intg->nchannels, 0,
			       sizeof(struct smintg_matrix)
			        * (nchannels - intg->nchannels));
		}
		intg->nchannels = nchannels;
		intg->matrix = matrix;
	}
	return 0;
}

#define SMATHSI_WSINC_0 0.859924743f
#define SMATHSI_WSINC_1 0.0853056678f
#define SMATHSI_WSINC_2 -0.0199242204f
#define SMATHSI_WSINC_3 0.00555994799f
#define SMATHSI_WSINC_4 -0.000883763365f

#define SMATHSI_LEAKINESS 0.995f

/**
 * Integrate the signal, parameterized version.
 */
static inline float smintg_do(struct smintg_matrix *matrix, float x) {
	float y = matrix->y1 * SMATHSI_LEAKINESS
	          + ((x + matrix->x8) * SMATHSI_WSINC_4
	          + (matrix->x1 + matrix->x7) * SMATHSI_WSINC_3
	          + (matrix->x2 + matrix->x6) * SMATHSI_WSINC_2
	          + (matrix->x3 + matrix->x5) * SMATHSI_WSINC_1
	          + matrix->x4 * SMATHSI_WSINC_0);

	matrix->x8 = matrix->x7;
	matrix->x7 = matrix->x6;
	matrix->x6 = matrix->x5;
	matrix->x5 = matrix->x4;
	matrix->x4 = matrix->x3;
	matrix->x3 = matrix->x2;
	matrix->x2 = matrix->x1;
	matrix->x1 = x;
	if(isnormal(y) || y == 0.0f) {
		matrix->y1 = y;
	} else if(y == INFINITY) {
		matrix->y1 = FLT_MAX;
	} else if(y == -INFINITY) {
		matrix->y1 = -FLT_MAX;
	} else {
		matrix->y1 = 0.0f;
	}
	return y;
}

/**
 * Integrate the incoming signal.
 */
static inline float smintg(struct smintg *intg, int channel,
                           float x) {
	return smintg_do(&intg->matrix[channel], x);
}

#endif /* ! SONICMATHS_INTEGRATOR_H */
