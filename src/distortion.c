/*
 * distortion.c
 * 
 * Copyright 2012 Evan Buswell
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

#include <math.h>
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/filter.h"
#include "sonicmaths/distortion.h"

static int smaths_distort_process(struct smaths_distort *self) {
    float *in_buffer = smaths_parameter_get_buffer(&self->in);
    if(in_buffer == NULL) {
	return -1;
    }
    float *gain_buffer = smaths_parameter_get_buffer(&self->gain);
    if(gain_buffer == NULL) {
	return -1;
    }
    float *sharpness_buffer = smaths_parameter_get_buffer(&self->sharpness);
    if(sharpness_buffer == NULL) {
	return -1;
    }
    float *out_buffer = gln_socket_get_buffer(&self->out);
    if(out_buffer == NULL) {
	return -1;
    }
    int kind = atomic_read(&self->kind);
    size_t i;
    for(i = 0; i < self->graph->graph.buffer_nmemb; i++) {
	float in = in_buffer[i];
	float sharpness = sharpness_buffer[i];
	float gain = gain_buffer[i];
	float factor;
	float E;
	int k;
	switch(kind) {
	case SMATHS_EXP:
	    factor = logf(expf(sharpness) + 1.0f);
	    if(in >= 0.0f) {
		out_buffer[i] = 1.0f
		    - logf(expf(-sharpness * ((in * gain) - 1.0f)) + 1.0f)
		    / factor;
	    } else {
		out_buffer[i] =
		    logf(expf(sharpness * ((in * gain) + 1.0f)) + 1.0f)
		    / factor
		    - 1.0;
	    }
	    break;
	case SMATHS_TUBE:
	    E = in;
	    for(k = 0; k < 5; k++) { /* five stages */
		float t;
	    	t = E * gain + 1.0f;
	    	E = powf((t * sqrtf(t * (9.0f * t - (4.0f * 0.01f)))
			  - (2.0f*0.01f*0.01f - 6.0f*0.01f*t + 3.0f*t*t))*0.01f/2.0f,
			 1.0f/3.0f);
	    	E = 1.27f * (E - ((2.0f*t - 0.01f) * 0.01f) / E + t - 0.01f) - 1.0f;
	    	if(isnanf(E) || (E < 1.0f)) {
	    	    E = -1.0f;
	    	} else if(E > 1.0f) {
	    	    E = 1.0f;
	    	}
	    }
	    out_buffer[i] = E;
	    break;
	case SMATHS_HYP:
	    out_buffer[i] =
		(in * gain)
		/ powf(powf(fabs(in * gain), sharpness) + 1.0, 1.0 / sharpness);
	    break;
	case SMATHS_ATAN:
	    out_buffer[i] =
		2.0f * atanf(sharpness * in * gain)
		/ M_PI;
	    break;
	default:
	    return -1;
	}
    }
    return 0;
}

int smaths_distort_init(struct smaths_distort *self, struct smaths_graph *graph) {
    int r;
    r = smaths_filter_init((struct smaths_filter *) self, graph, (gln_process_fp_t) smaths_distort_process, self);
    if(r != 0) {
	return r;
    }

    r = smaths_parameter_init(&self->gain, &self->node, 1.0f);
    if(r != 0) {
	smaths_filter_destroy((struct smaths_filter *) self);
	return r;
    }

    r = smaths_parameter_init(&self->sharpness, &self->node, 2.0f);
    if(r != 0) {
	smaths_parameter_destroy(&self->gain);
	smaths_filter_destroy((struct smaths_filter *) self);
	return r;
    }

    atomic_set(&self->kind, SMATHS_EXP);

    return 0;
}

void smaths_distort_destroy(struct smaths_distort *self) {
    smaths_parameter_destroy(&self->sharpness);
    smaths_parameter_destroy(&self->gain);
    smaths_filter_destroy((struct smaths_filter *) self);
}
