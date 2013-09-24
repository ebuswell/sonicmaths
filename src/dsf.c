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

static int smaths_dsf_process(struct smaths_dsf *dsf) {
    int r, i, j;

    struct smaths_buffer *freq_buffer;
    struct smaths_buffer *amp_buffer;
    struct smaths_buffer *phase_buffer;
    struct smaths_buffer *offset_buffer;
    struct smaths_buffer *bright_buffer;
    r = gln_get_buffers(5, &dsf->freq, &freq_buffer,
			&dsf->amp, &amp_buffer,
			&dsf->phase, &phase_buffer,
			&dsf->offset, &offset_buffer,
			&dsf->bright, &bright_buffer);
    if(r != 0) {
	return r;
    }

    float freq = smaths_parameter_fo(&dsf->freq, freq_buffer);
    float amp = smaths_parameter_fo(&dsf->amp, amp_buffer);
    float phase = smaths_parameter_fo(&dsf->phase, phase_buffer);
    float offset = smaths_parameter_fo(&dsf->offset, offset_buffer);
    float bright = smaths_parameter_fo(&dsf->bright, bright_buffer);

    int nframes = smaths_graph_frames_per_period((struct smaths_graph *) dsf->graph);
    int nchannels = 1;
    if((freq_buffer != NULL) && (freq_buffer->nchannels > nchannels)) {
	nchannels = freq_buffer->nchannels;
    }
    if((amp_buffer != NULL) && (amp_buffer->nchannels > nchannels)) {
	nchannels = amp_buffer->nchannels;
    }
    if((phase_buffer != NULL) && (phase_buffer->nchannels > nchannels)) {
	nchannels = phase_buffer->nchannels;
    }
    if((offset_buffer != NULL) && (offset_buffer->nchannels > nchannels)) {
	nchannels = offset_buffer->nchannels;
    }
    if((bright_buffer != NULL) && (bright_buffer->nchannels > nchannels)) {
	nchannels = bright_buffer->nchannels;
    }

    if(dsf->nchannels != nchannels) {
	float *t = arealloc(dsf->t, sizeof(float) * dsf->nchannels, sizeof(float) * nchannels);
	if(t == NULL) {
	    return -1;
	}
	for(i = dsf->nchannels; i < nchannels; i++) {
	    t[i] = t[i - 1];
	}
    }

    struct smaths_buffer *out_buffer = smaths_alloc_buffer(&dsf->out, nframes, nchannels);

    bool scale = atomic_load(&self->scale);

    for(i = 0; i < nframes; i++) {
	for(j = 0; j < nchannels; j++) {
	    while(dsf->t[j] >= 1.0f) {
		dsf->t[j] -= 1.0f;
	    }
	    float out;
	    float f = smaths_value(freq_buffer, i, j, freq);
	    float m = smaths_value(bright_buffer, i, j, bright);
	    float n = floorf(1 / (2 * f));
	    float na = (0.5 - n*f) / 0.0003;
	    float wt = 2 * M_PI * (dsf->t[j] + smaths_value(phase_buffer, i, j, phase));
	    if(scale) {
		/* the reason it's different when we scale is because
		 * eventually the peak is not visible within the
		 * sample resolution.  This value should keep
		 * everything within reasonable bounds from 15 hz on
		 * (at 44100kHz).  A higher value could be used for
		 * higher frequencies, but that makes this value
		 * frequency dependent, which doesn't sound good.  I
		 * suppose this could probably be coded according to
		 * sample-rate, but then that would make the
		 * brightness of 1.0 dependent on sample frequency in
		 * an unexpected way.  Formula is:
		 * (cos(4pi(15/sample_rate))
		 * /(1 + sin(4pi(15/sample_rate))) - 3.0517578125e-05 */
		m = m * 0.9957043154589819 + 3.0517578125e-05;
		if(n == 1.0f) {
		    out = sinf(wt) * (1 - m * m);
		} else {
		    out = (sinf(wt) - powf(m,n) * (sinf((n + 1) * wt) - m * sinf(n * wt)))
			/ (1 + m*m - 2*m*cosf(wt));
		    /* This should really switch to the other method
		       see itrain.c
		    */
		    if(na < 1.0f) {
			out -= (1 - L2ESCALE(na))*powf(m,n)*sinf(n * wt);
		    }
		    out *= (1 - m * m);
		}
	    } else {
		if(n == 1.0f) {
		    out = sinf(wt);
		} else {
		    out = (sinf(wt) - powf(m,n) * (sinf((n + 1) * wt) - m * sinf(n * wt)))
			/ (1 + m*m - 2*m*cosf(wt));
		    /* This should really switch to the other method
		       see itrain.c
		    */
		    if(na < 1.0f) {
			out -= (1 - L2ESCALE(na)) * powf(m,n) * sinf(n * wt);
		    }
		}
	    }
	    out_buffer[nchannels * i + j] = out * smaths_value(amp_buffer, i, j, amp)
		+ smaths_value(offset_buffer, i, j, offset);
	    self->t[j] += f;
	}
    }
    return 0;
}

int smaths_dsf_init(struct smaths_dsf *self, struct smaths_graph *graph) {
    int r;
    r = smaths_synth_init((struct smaths_synth *) self, graph, (gln_process_fp_t) smaths_dsf_process, self);
    if(r != 0) {
	return r;
    }
    r = smaths_parameter_init(&self->bright, &self->node, 0.0f);
    if(r != 0) {
	smaths_synth_destroy((struct smaths_synth *) self);
	return r;
    }
    atomic_store_explicit(&self->scale, false, memory_order_release);
    return 0;
}

void smaths_dsf_destroy(struct smaths_dsf *self) {
    smaths_parameter_destroy(&self->bright);
    smaths_synth_destroy((struct smaths_synth *) self);
}
