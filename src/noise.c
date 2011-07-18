/*
 * noise.c
 * 
 * Copyright 2011 Evan Buswell
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
#include <sonicmaths/noise.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>
#include <graphline.h>
#include <stdlib.h>
#include <math.h>

const float A[] = { 0.02109238, 0.07113478, 0.68873558 }; // rescaled by (1+P)/(1-P)
const float P[] = { 0.3190,  0.7756,  0.9613  };

struct float2 {
    float a;
    float b;
};

static inline struct float2 gaussian_random() {
    float s, u1, u2;
    struct float2 r;
    do {
	u1 = ((float) random()) / ((float) (RAND_MAX >> 1)) - 1.0f;
	u2 = ((float) random()) / ((float) (RAND_MAX >> 1)) - 1.0f;
	s = u1 * u1 + u2 * u2;
    } while(s >= 1.0);
    s = sqrtf(-2.0 * logf(s) / s);
    r.a = s * u1;
    r.b = s * u2;
    return r;
}

static inline float pink_random(float state[3]) {
    static const float RMI2 = 2.0 / ((float) RAND_MAX);
    static const float offset = 0.02109238 + 0.07113478 + 0.68873558;

    float temp = (float) random();
    state[0] = P[0] * (state[0] - temp) + temp;
    temp = (float) random();
    state[1] = P[1] * (state[1] - temp) + temp;
    temp = (float) random();
    state[2] = P[2] * (state[2] - temp) + temp;
    return ((A[0] * state[0] + A[1] * state[1] + A[2] * state[2]) * RMI2 - offset);
}

static int smaths_noise_process(struct smaths_noise *self) {
    float *out_buffer = gln_socket_get_buffer(&self->out);
    if(out_buffer == NULL) {
	return -1;
    }
    float *amp_buffer = smaths_parameter_get_buffer(&self->amp);
    if(amp_buffer == NULL) {
	return -1;
    }
    float *offset_buffer = smaths_parameter_get_buffer(&self->offset);
    if(offset_buffer == NULL) {
	return -1;
    }

    int kind = atomic_read(&self->kind);
    size_t i;
    struct float2 r;
    for(i = 0; i < self->graph->graph.buffer_nmemb; i++) {
	switch(kind) {
	case SMATHS_WHITE:
	    r = gaussian_random();
	    out_buffer[i] = r.a * amp_buffer[i] + offset_buffer[i];
	    i += 1;
	    out_buffer[i] = r.b * amp_buffer[i] + offset_buffer[i];
	    break;
	case SMATHS_PINK:
	    out_buffer[i] = pink_random(self->state) * amp_buffer[i] + offset_buffer[i];
	    break;
	case SMATHS_RED:
	    // not implemented
	default:
	    // unknown
	    return -1;
	}
    }
    return 0;
}

int smaths_noise_init(struct smaths_noise *self, struct smaths_graph *graph) {
    int r;

    self->graph = graph;
    self->state[0] = 0.0;
    self->state[1] = 0.0;
    self->state[2] = 0.0;
    atomic_set(&self->kind, SMATHS_WHITE);
    srandom((unsigned int) time(NULL));

    r = gln_node_init(&self->node, &graph->graph, (gln_process_fp_t) smaths_noise_process, self);
    if(r != 0) {
	return r;
    }

    r = gln_socket_init(&self->out, &self->node, OUTPUT);
    if(r != 0) {
	gln_node_destroy(&self->node);
	return r;
    }
    
    r = smaths_parameter_init(&self->amp, &self->node, 1.0f);
    if(r != 0) {
	gln_socket_destroy(&self->out);
	gln_node_destroy(&self->node);
	return r;
    }

    r = smaths_parameter_init(&self->offset, &self->node, 0.0f);
    if(r != 0) {
	smaths_parameter_destroy(&self->amp);
	gln_socket_destroy(&self->out);
	gln_node_destroy(&self->node);
	return r;
    }

    return 0;
}

void smaths_noise_destroy(struct smaths_noise *self) {
    smaths_parameter_destroy(&self->offset);
    smaths_parameter_destroy(&self->amp);
    gln_socket_destroy(&self->out);
    gln_node_destroy(&self->node);
}
