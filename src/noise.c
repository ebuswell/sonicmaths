/*
 * noise.c
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
#include <atomickit/atomic.h>
#include <atomickit/atomic-rcp.h>
#include <atomickit/atomic-malloc.h>
#include <graphline.h>
#include "sonicmaths/buffer.h"
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/noise.h"
#include "mtrand.h"

struct float2 {
    float a;
    float b;
};

static inline struct float2 gaussian_random() {
    float s, u1, u2;
    struct float2 r;
    do {
	u1 = mt_rand_float();
	u2 = mt_rand_float();
	s = u1 * u1 + u2 * u2;
    } while(s >= 1);
    s = sqrtf(-2 * logf(s) / s);
    r.a = s * u1;
    r.b = s * u2;
    return r;
}

static int smaths_noise_process(struct smaths_noise *noise) {
    int r, i, j;

    struct smaths_buffer *amp_buffer;
    float amp;
    struct smaths_buffer *offset_buffer;
    float offset;
    struct smaths_buffer *out_buffer;
    enum smaths_noise_kind kind;

    int nframes;
    int nchannels;

    r = gln_get_buffers(2, noise->amp, &amp_buffer,
			noise->offset, &offset_buffer);
    if(r != 0) {
	return r;
    }

    amp = smaths_parameter_go(noise->amp, amp_buffer);
    offset = smaths_parameter_go(noise->offset, offset_buffer);

    nframes = smaths_node_frames_per_period(noise);
    if(nframes < 0) {
	return nframes;
    }

    nchannels = SMATHS_MAX_NCHANNELS(2, amp_buffer, offset_buffer);

    out_buffer = smaths_alloc_buffer(noise->out, nframes, nchannels);
    if(out_buffer == NULL) {
	return -1;
    }

    kind = atomic_load_explicit(&noise->kind, memory_order_consume);

    float prev = NAN;
    for(i = 0; i < nframes; i++) {
	for(j = 0; j < nchannels; j++) {
	    float out;
	    switch(kind) {
	    case SMATHSN_GAUSSIAN:
		if(!isnan(prev)) {
		    out = prev;
		    prev = NAN;
		} else {
		    struct float2 r = gaussian_random();
		    out = r.a;
		    prev = r.b;
		}
		break;
	    case SMATHSN_UNIFORM:
	    default:
		out = mt_rand_float();
	    }
	    out_buffer->data[i * nchannels + j] = out * smaths_value(amp_buffer, i, j, amp) + smaths_value(offset_buffer, i, j, offset);
	}
    }

    return 0;
}

int smaths_noise_init(struct smaths_noise *noise, struct smaths_graph *graph, void (*destroy)(struct smaths_noise *)) {
    int r;

    r = gln_node_init(noise, graph, (gln_process_fp_t) smaths_noise_process, (void (*)(struct gln_node *)) destroy);
    if(r != 0) {
	goto undo0;
    }

    noise->out = gln_socket_create(noise, GLNS_OUTPUT);
    if(noise->out == NULL) {
	r = -1;
	goto undo1;
    }
    
    noise->amp = smaths_parameter_create(noise, 1.0f);
    if(noise->amp == NULL) {
	r = -1;
	goto undo2;
    }

    noise->offset = smaths_parameter_create(noise, 0.0f);
    if(noise->offset == NULL) {
	r = -1;
	goto undo3;
    }

    atomic_store(&noise->kind, SMATHSN_GAUSSIAN);

    return 0;

undo3:
    arcp_release(noise->amp);
undo2:
    arcp_release(noise->out);
undo1:
    gln_node_destroy(noise);
undo0:
    return r;
}

void smaths_noise_destroy(struct smaths_noise *noise) {
    arcp_release(noise->offset);
    arcp_release(noise->amp);
    arcp_release(noise->out);
    gln_node_destroy(noise);
}

static void _smaths_noise_destroy(struct smaths_noise *noise) {
    smaths_noise_destroy(noise);
    afree(noise, sizeof(struct smaths_noise));
}

struct smaths_noise *smaths_noise_create(struct smaths_graph *graph) {
    struct smaths_noise *ret;
    int r;

    ret = amalloc(sizeof(struct smaths_noise));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_noise_init(ret, graph, _smaths_noise_destroy);
    if(r != 0) {
	afree(ret, sizeof(struct smaths_noise));
	return NULL;
    }

    return ret;
}
