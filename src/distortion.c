/*
 * distortion.c
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

#include <math.h>
#include <atomickit/atomic.h>
#include <atomickit/atomic-rcp.h>
#include <atomickit/atomic-malloc.h>
#include <graphline.h>
#include "sonicmaths/buffer.h"
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/filter.h"
#include "sonicmaths/distortion.h"

static int smaths_distort_process(struct smaths_distort *distort) {
    int r, i, j;

    struct smaths_buffer *in_buffer;
    struct smaths_buffer *gain_buffer;
    struct smaths_buffer *sharpness_buffer;

    r = gln_get_buffers(3, distort->in, &in_buffer,
			distort->gain, &gain_buffer,
			distort->sharpness, &sharpness_buffer);
    if(r != 0) {
	return r;
    }

    float in = smaths_parameter_go(distort->in, in_buffer);
    float gain = smaths_parameter_go(distort->gain, gain_buffer);
    float sharpness = smaths_parameter_go(distort->sharpness, sharpness_buffer);

    int nframes = smaths_node_frames_per_period(distort);
    if(nframes < 0) {
	return nframes;
    }

    int nchannels = SMATHS_MAX_NCHANNELS(3, in_buffer, gain_buffer, sharpness_buffer);

    struct smaths_buffer *out_buffer = smaths_alloc_buffer(distort->out, nframes, nchannels);
    if(out_buffer == NULL) {
	return -1;
    }

    int kind = atomic_load_explicit(&distort->kind, memory_order_consume);

    for(i = 0; i < nframes; i++) {
	for(j = 0; j < nchannels; j++) {
	    in = smaths_value(in_buffer, i, j, in);
	    sharpness = smaths_value(sharpness_buffer, i, j, sharpness);
	    gain = smaths_value(gain_buffer, i, j, gain);
	    switch(kind) {
	    case SMATHSD_EXP: {
		float factor = logf(expf(sharpness) + 1);
		if(in >= 0.0f) {
		    out_buffer->data[nchannels * i + j] = 1
			- logf(expf(-sharpness * ((in * gain) - 1)) + 1)
			/ factor;
		} else {
		    out_buffer->data[nchannels * i + j] =
			logf(expf(sharpness * ((in * gain) + 1)) + 1)
			/ factor
			- 1;
		}
		break;
	    }
	    /* case SMATHS_TUBE: */
	    /* E = in; */
	    /* for(k = 0; k < 5; k++) { /\* five stages *\/ */
	    /* 	float t; */
	    /* 	t = E * gain + 1.0f; */
	    /* 	E = powf((t * sqrtf(t * (9.0f * t - (4.0f * 0.01f))) */
	    /* 		  - (2.0f*0.01f*0.01f - 6.0f*0.01f*t + 3.0f*t*t))*0.01f/2.0f, */
	    /* 		 1.0f/3.0f); */
	    /* 	E = 1.27f * (E - ((2.0f*t - 0.01f) * 0.01f) / E + t - 0.01f) - 1.0f; */
	    /* 	if(isnanf(E) || (E < 1.0f)) { */
	    /* 	    E = -1.0f; */
	    /* 	} else if(E > 1.0f) { */
	    /* 	    E = 1.0f; */
	    /* 	} */
	    /* } */
	    /* out_buffer->data[nchannels * i + j] = E; */
	    /* break; */
	    case SMATHSD_HYP:
		out_buffer->data[nchannels * i + j] =
		    (in * gain)
		    / powf(powf(fabs(in * gain), sharpness) + 1, 1 / sharpness);
		break;
	    case SMATHSD_ATAN:
		out_buffer->data[nchannels * i + j] =
		    2 * atanf(sharpness * in * gain)
		    / ((float) M_PI);
		break;
	    default:
		return -1;
	    }
	}
    }
    return 0;
}

int smaths_distort_init(struct smaths_distort *distort, struct smaths_graph *graph, void (*destroy)(struct smaths_distort *)) {
    int r;
    r = smaths_filter_init(distort, graph, (gln_process_fp_t) smaths_distort_process, (void (*)(struct smaths_filter *)) destroy);
    if(r != 0) {
	goto undo0;
    }

    distort->gain = smaths_parameter_create(distort, 1.0f);
    if(distort->gain == NULL) {
	r = -1;
	goto undo1;
    }

    distort->sharpness = smaths_parameter_create(distort, 2.0f);
    if(distort->sharpness == NULL) {
	r = -1;
	goto undo2;
    }

    atomic_store_explicit(&distort->kind, SMATHSD_EXP, memory_order_release);

    return 0;

undo2:
    arcp_release(distort->gain);
undo1:
    smaths_filter_destroy(distort);
undo0:
    return r;
}

void smaths_distort_destroy(struct smaths_distort *distort) {
    arcp_release(distort->sharpness);
    arcp_release(distort->gain);
    smaths_filter_destroy(distort);
}

static void _smaths_distort_destroy(struct smaths_distort *distort) {
    smaths_distort_destroy(distort);
    afree(distort, sizeof(struct smaths_distort));
}

struct smaths_distort *smaths_distort_create(struct smaths_graph *graph) {
    int r;
    struct smaths_distort *ret;

    ret = amalloc(sizeof(struct smaths_distort));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_distort_init(ret, graph, _smaths_distort_destroy);
    if(r != 0) {
	return NULL;
    }

    return ret;
}
