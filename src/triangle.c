/*
 * triangle.c
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
#include "sonicmaths/triangle.h"
#include "util.h"

#define TRIANGLE_SCALE_A 2.4674011002723395f
#define TRIANGLE_SCALE_B 1.939917501246181f
#define TRIANGLE_SCALE_C 0.5977211868262962f
#define TRIANGLE_SCALE_D 2.3561444022741096f

static int smaths_triangle_process(struct smaths_triangle *self) {
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
    float *skew_buffer = smaths_parameter_get_buffer(&self->skew);
    if(skew_buffer == NULL) {
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
	    float skew = skew_buffer[i];
	    while(skew > 1.0) {
		skew -= 1.0;
	    }
	    float out1 = smaths_itrain_do(f, self->synth.t + phase_buffer[i]);
	    float out2 = smaths_itrain_do(f, self->synth.t + phase_buffer[i] + skew);

	    out1 = smaths_do_integral(&self->intg11_matrix, out1);
	    out1 = smaths_do_integral(&self->intg12_matrix, out1);
	    out2 = smaths_do_integral(&self->intg21_matrix, out2);
	    out2 = smaths_do_integral(&self->intg22_matrix, out2);

	    out1 *= 4.0f * M_PI * M_PI * f * f;
	    out2 *= 4.0f * M_PI * M_PI * f * f;
	    out1 -= out2;

	    if(scale) {
		/* Scales according to a parabola which is an
		 * approximate fit for the maximum value of the
		 * waveform along the range of skew values. */
		if(skew < 0.5f) {
		    skew = 1.0f - skew;
		}
		float M = 1.0f / (2.0f * f);
		out1 /= TRIANGLE_SCALE_A
		    - TRIANGLE_SCALE_B / (M + TRIANGLE_SCALE_C)
		    + TRIANGLE_SCALE_B / (2.0f * M + TRIANGLE_SCALE_C);
		out1 /= 1.0f - powf(2.0f * skew - 1.0f, TRIANGLE_SCALE_D);
	    }

	    out_buffer[i] = out1 * amp_buffer[i] + offset_buffer[i];
	    self->synth.t += f;
	}
    }
    return 0;
}

int smaths_triangle_init(struct smaths_triangle *self, struct smaths_graph *graph) {
    int r;
    r = smaths_synth_init(&self->synth, graph, (gln_process_fp_t) smaths_triangle_process, self);
    if(r != 0) {
	return r;
    }
    r = smaths_parameter_init(&self->skew, &self->synth.node, 0.5f);
    if(r != 0) {
	smaths_synth_destroy(&self->synth);
	return r;
    }
    atomic_set(&self->scale, 0);
    memset(&self->intg11_matrix, 0, sizeof(struct smaths_intg_matrix));
    memset(&self->intg12_matrix, 0, sizeof(struct smaths_intg_matrix));
    memset(&self->intg21_matrix, 0, sizeof(struct smaths_intg_matrix));
    memset(&self->intg22_matrix, 0, sizeof(struct smaths_intg_matrix));
    return 0;
}

void smaths_triangle_destroy(struct smaths_triangle *self) {
    smaths_parameter_destroy(&self->skew);
    smaths_synth_destroy(&self->synth);
}
