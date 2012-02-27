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

#define M_MNTS 9634303.96f
#define M_MNTS_NSQR 0.000322173509f

static int smaths_itrain_process(struct smaths_itrain *self) {
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
	    float n = floorf(1.0f / (2.0f * f)); /* the number of harmonics */
	    float wt_2 = M_PI * (self->synth.t + phase_buffer[i]); /* half angular frequency */
	    float m_f = powf(M_MNTS, f);
	    float out = sinf(n * wt_2) * cosf((n + 1.0f) * wt_2)
		/ sinf(wt_2);
	    /* adjust top harmonics such that new harmonics gradually rise from 0 */
	    out -= m_f * M_MNTS_NSQR
		* (cosf(2.0f * wt_2)
		   - powf(m_f, n) * cosf((n + 1.0f) * 2.0f * wt_2)
		   + powf(m_f, n + 1) * cosf(n * 2.0f * wt_2)
		   - m_f
		    )
		/ (1.0f + m_f * m_f - 2.0f * m_f * cosf(2.0f * wt_2));

	    if(scale) {
		out *= 2.0 * f;
	    }

	    out_buffer[i] = out * amp_buffer[i] + offset_buffer[i];
	    self->synth.t += f;
	}
    }
    return 0;
}

int smaths_itrain_init(struct smaths_itrain *self, struct smaths_graph *graph) {
    atomic_set(&self->scale, 1);
    return smaths_synth_init(&self->synth, graph, (gln_process_fp_t) smaths_itrain_process, self);
}
