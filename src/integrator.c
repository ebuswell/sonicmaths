/*
 * integrator.c
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
#include <atomickit/atomic-float.h>
#include <atomickit/atomic-malloc.h>
#include <graphline.h>
#include "sonicmaths/graph.h"
#include "sonicmaths/filter.h"
#include "sonicmaths/integrator.h"

static inline int smaths_integrator_redim_state(struct smaths_integrator *integrator, int nchannels) {
    if(nchannels != integrator->nchannels) {
	struct smaths_intg_matrix *intg_matrix = arealloc(integrator->intg_matrix,
							  sizeof(struct smaths_intg_matrix) * integrator->nchannels,
							  sizeof(struct smaths_intg_matrix) * nchannels);
	if(intg_matrix == NULL) {
	    return -1;
	}
	if(nchannels > integrator->nchannels) {
	    memset(intg_matrix + integrator->nchannels, 0, sizeof(struct smaths_intg_matrix) * (nchannels - integrator->nchannels));
	}
	integrator->intg_matrix = intg_matrix;
	integrator->nchannels = nchannels;
    }
    return 0;
}

static int smaths_integrator_process(struct smaths_integrator *integrator) {
    int r, i, j;

    struct smaths_buffer *in_buffer;

    r = gln_get_buffers(1, integrator->in, &in_buffer);
    if(r != 0) {
	return r;
    }

    float in = smaths_parameter_go(integrator->in, in_buffer);

    int nframes = smaths_node_frames_per_period(integrator);
    if(nframes < 0) {
	return nframes;
    }

    int nchannels = in_buffer == NULL ? 1 : in_buffer->nchannels;

    r = smaths_integrator_redim_state(integrator, nchannels);
    if(r != 0) {
	return r;
    }

    struct smaths_buffer *out_buffer = smaths_alloc_buffer(integrator->out, nframes, nchannels);
    if(out_buffer == NULL) {
	return -1;
    }

    for(i = 0; i < nframes; i++) {
	for(j = 0; j < nchannels; j++) {
	    out_buffer->data[i * nchannels + j] = smaths_do_integral(&integrator->intg_matrix[j], smaths_value(in_buffer, i, j, in));
	}
    }

    return 0;
}

int smaths_integrator_init(struct smaths_integrator *integrator, struct smaths_graph *graph, void (*destroy)(struct smaths_integrator *)) {
    int r = -1;

    integrator->intg_matrix = amalloc(sizeof(struct smaths_intg_matrix));
    if(integrator->intg_matrix == NULL) {
	goto undo0;
    }

    r = smaths_filter_init(integrator, graph, (gln_process_fp_t) smaths_integrator_process, (void (*)(struct smaths_filter *)) destroy);
    if(r != 0) {
	goto undo1;
    }

    memset(integrator->intg_matrix, 0, sizeof(struct smaths_intg_matrix));
    integrator->nchannels = 1;

    return 0;

undo1:
    afree(integrator->intg_matrix, sizeof(struct smaths_intg_matrix));
undo0:
    return r;
}

void smaths_integrator_destroy(struct smaths_integrator *integrator) {
    afree(integrator->intg_matrix, integrator->nchannels * sizeof(struct smaths_intg_matrix));
    smaths_filter_destroy(integrator);
}

static void __smaths_integrator_destroy(struct smaths_integrator *integrator) {
    smaths_integrator_destroy(integrator);
    afree(integrator, sizeof(struct smaths_integrator));
}

struct smaths_integrator *smaths_integrator_create(struct smaths_graph *graph) {
    struct smaths_integrator *ret;
    int r;

    ret = amalloc(sizeof(struct smaths_integrator));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_integrator_init(ret, graph, __smaths_integrator_destroy);
    if(r != 0) {
	return NULL;
    }

    return ret;
}
