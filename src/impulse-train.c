/*
 * impulse_train.c
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
#include <math.h>
#include <atomickit/atomic.h>
#include <graphline.h>
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/synth.h"
#include "sonicmaths/impulse-train.h"
#include "util.h"

static int smaths_itrain_process(struct smaths_itrain *self) {
    float *freq_buffer = smaths_parameter_get_buffer(&self->freq);
    if(freq_buffer == NULL) {
	return -1;
    }
    float *amp_buffer = smaths_parameter_get_buffer(&self->amp);
    if(amp_buffer == NULL) {
	return -1;
    }
    float *phase_buffer = smaths_parameter_get_buffer(&self->phase);
    if(phase_buffer == NULL) {
	return -1;
    }
    float *offset_buffer = smaths_parameter_get_buffer(&self->offset);
    if(offset_buffer == NULL) {
	return -1;
    }
    float *out_buffer = gln_socket_get_buffer(&self->out);
    if(out_buffer == NULL) {
	return -1;
    }
    int scale = atomic_read(&self->scale);
    size_t i;
    for(i = 0; i < self->graph->graph.buffer_nmemb; i++) {
	float f = freq_buffer[i];
	if(isnanf(f)) {
	    self->t = 0.0;
	    out_buffer[i] = offset_buffer[i];
	} else {
	    if(self->t >= 1.0) {
		self->t -= 1.0;
	    }
	    float out = smaths_itrain_do(f, self->t + phase_buffer[i]);

	    if(scale) {
		out *= 2.0 * f;
	    }

	    out_buffer[i] = out * amp_buffer[i] + offset_buffer[i];
	    self->t += f;
	}
    }
    return 0;
}

int smaths_itrain_init(struct smaths_itrain *self, struct smaths_graph *graph) {
    atomic_set(&self->scale, 0);
    return smaths_synth_init((struct smaths_synth *) self, graph, (gln_process_fp_t) smaths_itrain_process, self);
}
