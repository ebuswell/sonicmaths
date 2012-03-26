/*
 * square.c
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
#include "sonicmaths/square.h"
#include "util.h"

#define SQUARE_SCALE_A 3.7056635625712766f
#define SQUARE_SCALE_B 1.4414686477746828f
#define SQUARE_SCALE_C 1.0790320109533682f

static int smaths_square_process(struct smaths_square *self) {
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
    float *skew_buffer = smaths_parameter_get_buffer(&self->skew);
    if(skew_buffer == NULL) {
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
	    float skew = skew_buffer[i];
	    while(skew > 1.0) {
		skew -= 1.0;
	    }
	    float out1 = smaths_itrain_do(f, self->t + phase_buffer[i]);
	    float out2 = smaths_itrain_do(f, self->t + phase_buffer[i] + skew);

	    out1 = smaths_do_integral(&self->intg1_matrix, out1);
	    out1 *= 2.0f * M_PI * f;

	    out2 = smaths_do_integral(&self->intg2_matrix, out2);
	    out2 *= 2.0f * M_PI * f;

	    out1 -= out2;

	    if(scale) {
		/* Scales according to a hyperbola which is an
		 * approximate fit for the maximum value of the
		 * waveform along the range of skew values. */
		if(skew < 0.5) {
		    skew = 1.0 - skew;
		}
		float M = 1 / (2.0 * f);
		float oy = SQUARE_SCALE_A + SQUARE_SCALE_B / (M + SQUARE_SCALE_C);
		out1 /= oy;
		oy = M_PI / oy;
		float oxm = 1.0f / (2.0f * (M + 1.0f));
		skew -= 1.0f;
		skew += oxm;
		M *= 2.0f;
		float dd = (2.0f * oy + M) * oxm + 1.0f;
		float sxsq = (oy + M) * skew / dd;
		out1 /= 1.0f - (M - oy * oy * oxm) * skew / dd - (oy * oxm + 1.0f) * sqrtf(sxsq * sxsq + (1.0f - M * oxm) / dd);
	    }

	    out_buffer[i] = out1 * amp_buffer[i] + offset_buffer[i];
	    self->t += f;
	}
    }
    return 0;
}

int smaths_square_init(struct smaths_square *self, struct smaths_graph *graph) {
    int r;
    r = smaths_synth_init((struct smaths_synth *) self, graph, (gln_process_fp_t) smaths_square_process, self);
    if(r != 0) {
	return r;
    }
    r = smaths_parameter_init(&self->skew, &self->node, 0.5f);
    if(r != 0) {
	smaths_synth_destroy((struct smaths_synth *) self);
	return r;
    }
    atomic_set(&self->scale, 0);
    memset(&self->intg1_matrix, 0, sizeof(struct smaths_intg_matrix));
    memset(&self->intg2_matrix, 0, sizeof(struct smaths_intg_matrix));
    return 0;
}

void smaths_square_destroy(struct smaths_square *self) {
    smaths_parameter_destroy(&self->skew);
    smaths_synth_destroy((struct smaths_synth *) self);
}
