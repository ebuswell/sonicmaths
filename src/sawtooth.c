/*
 * sawtooth.c
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
#include <string.h>
#include <atomickit/atomic.h>
#include <graphline.h>
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/synth.h"
#include "sonicmaths/impulse-train.h"
#include "sonicmaths/sawtooth.h"
#include "util.h"

#define SAW_SCALE_A 1.851937052f
#define SAW_SCALE_B 1.570830481f
#define SAW_SCALE_C 0.8438339747f

static int smaths_saw_process(struct smaths_saw *self) {
    float *freq_buffer = smaths_parameter_get_buffer(&self->synth.freq);
    if(freq_buffer == NULL) {
	return -1;
    }
    float *amp_buffer = smaths_parameter_get_buffer(&self->synth.amp);
    if(amp_buffer == NULL) {
	return -1;
    }
    float *phase_buffer = smaths_parameter_get_buffer(&self->synth.phase);
    if(phase_buffer == NULL) {
	return -1;
    }
    float *offset_buffer = smaths_parameter_get_buffer(&self->synth.offset);
    if(offset_buffer == NULL) {
	return -1;
    }
    float *out_buffer = gln_socket_get_buffer(&self->synth.out);
    if(out_buffer == NULL) {
	return -1;
    }
    int scale = atomic_read(&self->scale);
    size_t i;
    for(i = 0; i < self->synth.graph->graph.buffer_nmemb; i++) {
	float f = freq_buffer[i];
	if(isnanf(f)) {
	    self->synth.t = 0.0;
	    out_buffer[i] = offset_buffer[i];
	} else {
	    if(self->synth.t >= 1.0) {
		self->synth.t -= 1.0;
	    }
	    float out = smaths_itrain_do(f, self->synth.t + phase_buffer[i]);

	    out = smaths_do_integral(&self->intg_matrix, out);
	    out *= 2.0f * M_PI * f;

	    if(scale) {
		out /= SAW_SCALE_A - SAW_SCALE_B
		                   / (1 / (2.0f * f) + SAW_SCALE_C);
	    }

	    out_buffer[i] = out * amp_buffer[i] + offset_buffer[i];
	    self->synth.t += f;
	}
    }
    return 0;
}

int smaths_saw_init(struct smaths_saw *self, struct smaths_graph *graph) {
    atomic_set(&self->scale, 0);
    memset(&self->intg_matrix, 0, sizeof(struct smaths_intg_matrix));
    return smaths_synth_init(&self->synth, graph, (gln_process_fp_t) smaths_saw_process, self);
}
