/*
 * sine.c
 * 
 * Copyright 2010 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 2 of the License,
 * or (at your option) any later version.
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
#include "sonicmaths/parameter.h"
#include "sonicmaths/synth.h"
#include "sonicmaths/sine.h"

static int smaths_sine_process(struct smaths_sine *self) {
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
    size_t i;
    for(i = 0; i < self->graph->graph.buffer_nmemb; i++) {
	if(isnanf(freq_buffer[i])) {
	    self->t = 0.0;
	    out_buffer[i] = offset_buffer[i];
	} else {
	    while(self->t >= 1.0) {
		self->t -= 1.0;
	    }
	    out_buffer[i] = sinf(2.0f * M_PI * (self->t + phase_buffer[i])) * amp_buffer[i] + offset_buffer[i];
	    self->t += (double) freq_buffer[i];
	}
    }
    return 0;
}

int smaths_sine_init(struct smaths_sine *self, struct smaths_graph *graph) {
    return smaths_synth_init((struct smaths_synth *) self, graph, (gln_process_fp_t) smaths_sine_process, self);
}
