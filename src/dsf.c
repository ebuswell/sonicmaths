/*
 * dsf.c Discrete Summation Formula
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
#include "sonicmaths/parameter.h"
#include "sonicmaths/synth.h"
#include "sonicmaths/dsf.h"
#include "util.h"

static int smaths_dsf_process(struct smaths_dsf *self) {
    float *freq_buffer = smaths_parameter_get_buffer(&self->synth.freq);
    if(freq_buffer == NULL) {
	return -1;
    }
    float *bright_buffer = smaths_parameter_get_buffer(&self->bright);
    if(bright_buffer == NULL) {
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
    bool scale = (bool) atomic_read(&self->scale);
    size_t i;
    for(i = 0; i < self->synth.graph->graph.buffer_nmemb; i++) {
        float f = freq_buffer[i];
	float m = bright_buffer[i];
	if(f == 0.0f || isnanf(f)) {
	    self->synth.t = 0.0f;
	    out_buffer[i] = offset_buffer[i];
	} else {
	    if(self->synth.t >= 1.0f) {
		self->synth.t -= 1.0f;
	    }
	    float out;
	    float n = floorf(1.0 / (2.0 * f));
	    float na = (0.5 - n*f) / 0.0003;
	    float wt = 2.0 * M_PI * (self->synth.t + phase_buffer[i]);
	    if(scale) {
		/* the reason it's different when we scale is because eventually the peak
		 * is not visible within the sample resolution.  This value should keep
		 * everything within reasonable bounds from 15 hz on (at 44100kHz).  A
		 * higher value could be used for higher frequencies, but that makes this
		 * value frequency dependent, which doesn't sound good.  I suppose this
		 * could probably be coded according to sample-rate, but then that would
		 * make the brightness of 1.0 dependent on sample frequency in an
		 * unexpected way.  Formula is (cos(4pi(15/sample_rate))/(1 +
		 * sin(4pi(15/sample_rate))) - 3.0517578125e-05 */
		m = m * 0.9957043154589819 + 3.0517578125e-05;
		if(n == 1.0f) {
		    out = sinf(wt) * (1.0f - m * m);
		} else {
		    out = (sinf(wt) - powf(m,n) * (sinf((n + 1.0) * wt) - m * sinf(n * wt)))
			/ (1.0f + m*m - 2.0f*m*cosf(wt));
		    if(na < 1.0f) {
			out -= (1.0f - L2ESCALE(na))*powf(m,n)*sinf(n * wt);
		    }
		    out *= (1.0 - m * m);
		}
	    } else {
		if(n == 1.0) {
		    out = sinf(wt);
		} else {
		    out = (sinf(wt) - powf(m,n) * (sinf((n + 1.0) * wt) - m * sinf(n * wt)))
			/ (1.0f + m*m - 2.0f*m*cosf(wt));
		    if(na < 1.0f) {
			out -= (1.0f - L2ESCALE(na)) * powf(m,n) * sinf(n * wt);
		    }
		}
	    }
	    out_buffer[i] = out * amp_buffer[i] + offset_buffer[i];
	    self->synth.t += f;
	}
    }
    return 0;
}

int smaths_dsf_init(struct smaths_dsf *self, struct smaths_graph *graph) {
    int r;
    r = smaths_synth_init(&self->synth, graph, (gln_process_fp_t) smaths_dsf_process, self);
    if(r != 0) {
	return r;
    }
    r = smaths_parameter_init(&self->bright, &self->synth.node, 0.0f);
    if(r != 0) {
	smaths_synth_destroy(&self->synth);
	return r;
    }
    atomic_set(&self->scale, 1);
    return 0;
}

void smaths_dsf_destroy(struct smaths_dsf *self) {
    smaths_parameter_destroy(&self->bright);
    smaths_synth_destroy(&self->synth);
}
