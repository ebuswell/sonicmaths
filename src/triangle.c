/*
 * triangle.c
 * 
 * Copyright 2013 Evan Buswell
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
#include "sonicmaths/buffer.h"
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/synth.h"
#include "sonicmaths/impulse-train.h"
#include "sonicmaths/triangle.h"
/*#include "util.h"*/

#define TRIANGLE_SCALE_A 2.4674011002723395f
#define TRIANGLE_SCALE_B 1.939917501246181f
#define TRIANGLE_SCALE_C 0.5977211868262962f
#define TRIANGLE_SCALE_D 2.3561444022741096f

static inline int smaths_triangle_redim_state(struct smaths_triangle *triangle, int nchannels) {
    if(nchannels != triangle->nchannels) {
	int i;
	float *t;
	struct smaths_intg_matrix *intg_matrix;

	t = arealloc(triangle->t, sizeof(float) * triangle->nchannels,
		     sizeof(float) * nchannels);
	if(t == NULL) {
	    return -1;
	}

	intg_matrix = arealloc(triangle->intg_matrix, sizeof(struct smaths_intg_matrix) * triangle->nchannels * 4,
			       sizeof(struct smaths_intg_matrix) * nchannels * 4);
	if(intg_matrix == NULL) {
	    afree(triangle->intg_matrix, sizeof(struct smaths_intg_matrix) * triangle->nchannels * 4);
	    afree(t, sizeof(float) * nchannels);
	    triangle->nchannels = 0;
	}
	if(triangle->nchannels < nchannels) {
	    for(i = triangle->nchannels; i < nchannels; i++) {
		t[i] = t[i - 1];
	    }
	    memset(intg_matrix + triangle->nchannels * 4, 0, sizeof(struct smaths_intg_matrix) * (nchannels - triangle->nchannels) * 4);
	}
	triangle->nchannels = nchannels;
	triangle->intg_matrix = intg_matrix;
	triangle->t = t;
    }
    return 0;
}

static inline float smaths_triangle_process_stage(float f, float t, float amp, float phase, float offset, float skew, bool scale, struct smaths_intg_matrix *intg_matrix) {
    float out1 = smaths_do_itrain(f, t + phase);
    float out2 = smaths_do_itrain(f, t + phase + skew);
    out1 = smaths_do_integral(&intg_matrix[0], out1);
    out1 = smaths_do_integral(&intg_matrix[1], out1);
    out2 = smaths_do_integral(&intg_matrix[2], out2);
    out2 = smaths_do_integral(&intg_matrix[3], out2);

    out1 -= out2;

    out1 *= ((float) (4 * M_PI * M_PI)) * f * f;

    if(scale) {
	/* Scales according to a parabola which is an
	 * approximate fit for the maximum value of the
	 * waveform along the range of skew values. */
	skew = fmodf(skew, 1);
	if(skew < 0) {
	    skew = 1 + skew;
	}
	if(skew < 0.5f) {
	    skew = 1 - skew;
	}
	float M = 1 / (2 * f);
	out1 /= TRIANGLE_SCALE_A
	    - TRIANGLE_SCALE_B / (M + TRIANGLE_SCALE_C)
	    + TRIANGLE_SCALE_B / (2 * M + TRIANGLE_SCALE_C);
	out1 /= 1 - powf(2 * skew - 1, TRIANGLE_SCALE_D);
    }

    return out1 * amp + offset;
}

static int smaths_triangle_process(struct smaths_triangle *triangle) {
    int r, i, j;

    struct smaths_buffer *freq_buffer;
    float freq;
    struct smaths_buffer *amp_buffer;
    float amp;
    struct smaths_buffer *phase_buffer;
    float phase;
    struct smaths_buffer *offset_buffer;
    float offset;
    struct smaths_buffer *skew_buffer;
    float skew;
    struct smaths_buffer *out_buffer;
    int nframes, nchannels;

    r = gln_get_buffers(5, triangle->freq, &freq_buffer,
			triangle->amp, &amp_buffer,
			triangle->phase, &phase_buffer,
			triangle->offset, &offset_buffer,
			triangle->skew, &skew_buffer);
    if(r != 0) {
	return r;
    }

    freq = smaths_parameter_go(triangle->freq, freq_buffer);
    amp = smaths_parameter_go(triangle->amp, amp_buffer);
    phase = smaths_parameter_go(triangle->phase, phase_buffer);
    offset = smaths_parameter_go(triangle->offset, offset_buffer);
    skew = smaths_parameter_go(triangle->skew, skew_buffer);

    nframes = smaths_node_frames_per_period(triangle);
    if(nframes < 0) {
	return nframes;
    }

    nchannels = SMATHS_MAX_NCHANNELS(5, freq_buffer, amp_buffer, phase_buffer, offset_buffer, skew_buffer);

    r = smaths_triangle_redim_state(triangle, nchannels);
    if(r != 0) {
	return r;
    }

    out_buffer = smaths_alloc_buffer(triangle->out, nframes, nchannels);
    if(out_buffer == NULL) {
	return -1;
    }

    bool scale = atomic_load_explicit(&triangle->scale, memory_order_consume);

    for(i = 0; i < nframes; i++) {
	for(j = 0; j < nchannels; j++) {
	    if(triangle->t[j] >= 1.0f) {
		triangle->t[j] -= 1.0f;
	    }
	    float f = smaths_value(freq_buffer, i, j, freq);
	    out_buffer->data[i * nchannels + j] = smaths_triangle_process_stage(
		f,
		triangle->t[j],
		smaths_value(amp_buffer, i, j, amp),
		smaths_value(phase_buffer, i, j, phase),
		smaths_value(offset_buffer, i, j, offset),
		smaths_value(skew_buffer, i, j, skew),
		scale,
		&triangle->intg_matrix[j*2]);

	    triangle->t[j] += f;
	}
    }
    return 0;
}

int smaths_triangle_init(struct smaths_triangle *triangle, struct smaths_graph *graph, void (*destroy)(struct smaths_triangle *)) {
    int r = -1;

    atomic_init(&triangle->scale, false);

    triangle->intg_matrix = amalloc(sizeof(struct smaths_intg_matrix) * 4);
    if(triangle->intg_matrix == NULL) {
	goto undo0;
    }

    r = smaths_synth_init(triangle, graph, (gln_process_fp_t) smaths_triangle_process, (void (*)(struct smaths_synth *)) destroy);
    if(r != 0) {
	goto undo1;
    }

    triangle->skew = smaths_parameter_create(triangle, 0.5f);
    if(triangle->skew == NULL) {
	r = -1;
	goto undo2;
    }

    memset(triangle->intg_matrix, 0, sizeof(struct smaths_intg_matrix) * 4);
    return 0;

undo2:
    smaths_synth_destroy(triangle);
undo1:
    afree(triangle->intg_matrix, sizeof(struct smaths_intg_matrix) * 4);
undo0:
    return -1;
}

void smaths_triangle_destroy(struct smaths_triangle *triangle) {
    arcp_release(triangle->skew);
    smaths_synth_destroy(triangle);
    afree(triangle->intg_matrix, sizeof(struct smaths_intg_matrix) * triangle->nchannels * 4);
}

static void __smaths_triangle_destroy(struct smaths_triangle *triangle) {
    smaths_triangle_destroy(triangle);
    afree(triangle, sizeof(struct smaths_triangle));
}

struct smaths_triangle *smaths_triangle_create(struct smaths_graph *graph) {
    struct smaths_triangle *ret;
    int r;

    ret = amalloc(sizeof(struct smaths_triangle));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_triangle_init(ret, graph, __smaths_triangle_destroy);
    if(r != 0) {
	afree(ret, sizeof(struct smaths_triangle));
    }

    return ret;
}

