/*
 * parabola.c
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
#include "sonicmaths/parabola.h"
#include "util.h"

#define BOLA_SCALE_A 1.644934067f
#define BOLA_SCALE_B 1.00000009613580760f
#define BOLA_SCALE_C 0.550546246f

static int smaths_bola_process(struct smaths_bola *self) {
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

	    out = smaths_do_integral(&self->intg1_matrix, out);
	    out = smaths_do_integral(&self->intg2_matrix, out);

	    out *= 4.0f * M_PI * M_PI * f * f;

	    if(scale) {
		out /= BOLA_SCALE_A - BOLA_SCALE_B
		                   / (1 / (2.0f * f) + BOLA_SCALE_C);
	    }

	    out_buffer[i] = out * amp_buffer[i] + offset_buffer[i];
	    self->synth.t += f;
	}
    }
    return 0;
}

int smaths_bola_init(struct smaths_bola *self, struct smaths_graph *graph) {
    atomic_set(&self->scale, 0);
    memset(&self->intg1_matrix, 0, sizeof(struct smaths_intg_matrix));
    memset(&self->intg2_matrix, 0, sizeof(struct smaths_intg_matrix));
    self->intg2_matrix.y1 = -M_PI * M_PI / 6.0f;
    return smaths_synth_init(&self->synth, graph, (gln_process_fp_t) smaths_bola_process, self);
}
