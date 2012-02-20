/*
 * highpass.c
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
#include "sonicmaths/highpass.h"

static int smaths_highpass_process(struct smaths_highpass *self) {
    float *in_buffer = smaths_parameter_get_buffer(&self->lowpass.filter.in);
    if(in_buffer == NULL) {
	return -1;
    }
    float *freq_buffer = smaths_parameter_get_buffer(&self->lowpass.freq);
    if(freq_buffer == NULL) {
	return -1;
    }
    float *Q_buffer = smaths_parameter_get_buffer(&self->lowpass.Q);
    if(Q_buffer == NULL) {
	return -1;
    }
    float *out_buffer = gln_socket_get_buffer(&self->lowpass.filter.out);
    if(out_buffer == NULL) {
	return -1;
    }
    size_t i;
    for(i = 0; i < self->lowpass.filter.graph->graph.buffer_nmemb; i++) {
	float f = freq_buffer[i];
	if(f > 0.5) {
	    f = 0.5;
	}
	float x = in_buffer[i];
	float w = 2.0 * M_PI * f;
	float a = sinf(w)/(2*Q_buffer[i]);
	float cosw = cosf(w);
	float y = (((1.0 + cosw)/2)/(1.0 + a)) * x - ((1.0 + cosw)/(1.0 + a)) * self->lowpass.x1 + (((1.0 + cosw)/2)/(1.0 + a)) * self->lowpass.x2
	    + (2*cosw/(1.0 + a)) * self->lowpass.y1 - ((1 - a)/(1 + a)) * self->lowpass.y2;

	self->lowpass.x2 = self->lowpass.x1;
	self->lowpass.x1 = x;
	self->lowpass.y2 = self->lowpass.y1;
	if(y == INFINITY) {
	    self->lowpass.y1 = 1.0f;
	} else if (y == -INFINITY) {
	    self->lowpass.y1 = -1.0f;
	} else {
	    self->lowpass.y1 = y;
	}
	out_buffer[i] = y;
    }
    return 0;
}

int smaths_highpass_init(struct smaths_highpass *highpass, struct smaths_graph *graph) {
    return smaths_lowpass_subclass_init(&highpass->lowpass, graph, (gln_process_fp_t) smaths_highpass_process, highpass);
}
