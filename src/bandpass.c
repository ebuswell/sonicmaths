/*
 * bandpass.c
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
#include <atomickit/atomic-float.h>
#include <graphline.h>
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/filter.h"
#include "sonicmaths/lowpass.h"
#include "sonicmaths/bandpass.h"

static int smaths_bandpass_process(struct smaths_bandpass *self) {
    float *in_buffer = smaths_parameter_get_buffer(&self->in);
    if(in_buffer == NULL) {
	return -1;
    }
    float *freq_buffer = smaths_parameter_get_buffer(&self->freq);
    if(freq_buffer == NULL) {
	return -1;
    }
    float *Q_buffer = smaths_parameter_get_buffer(&self->Q);
    if(Q_buffer == NULL) {
	return -1;
    }
    float *out_buffer = gln_socket_get_buffer(&self->out);
    if(out_buffer == NULL) {
	return -1;
    }
    size_t i;
    for(i = 0; i < self->graph->graph.buffer_nmemb; i++) {
	float f = freq_buffer[i];
	if(f > 0.5f) {
	    f = 0.5f;
	}
	float x = in_buffer[i];
	float w = 2.0 * M_PI * f;
	float Q = Q_buffer[i];
	float a = sinf(w)/(2 * Q);
	float y = (a/(1.0f + a)) * x - (a/(1.0f + a)) * self->x2
	    + (2*cos(w)/(1.0f + a)) * self->y1 - ((1.0f - a)/(1.0f + a)) * self->y2;

	self->x2 = self->x1;
	self->x1 = x;
	self->y2 = self->y1;
	if(y == INFINITY) {
	    self->y1 = 1.0f;
	} else if (y == -INFINITY) {
	    self->y1 = -1.0f;
	} else {
	    self->y1 = y;
	}
	out_buffer[i] = y;
    }
    return 0;
}

int smaths_bandpass_init(struct smaths_bandpass *self, struct smaths_graph *graph) {
    return smaths_lowpass_subclass_init((struct smaths_lowpass *) self, graph, (gln_process_fp_t) smaths_bandpass_process, self);
}
