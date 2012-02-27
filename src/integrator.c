/*
 * integrator.c
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
#include <atomickit/atomic-float.h>
#include <graphline.h>
#include "sonicmaths/graph.h"
#include "sonicmaths/filter.h"
#include "sonicmaths/integrator.h"

static int smaths_integrator_process(struct smaths_integrator *self) {
    float *in_buffer = smaths_parameter_get_buffer(&self->filter.in);
    if(in_buffer == NULL) {
	return -1;
    }
    float *out_buffer = gln_socket_get_buffer(&self->filter.out);
    if(out_buffer == NULL) {
	return -1;
    }

    size_t i;
    for(i = 0; i < self->filter.graph->graph.buffer_nmemb; i++) {
	out_buffer[i] = smaths_do_integral(&self->intg_matrix, in_buffer[i]);
    }
    return 0;
}

int smaths_integrator_init(struct smaths_integrator *integrator, struct smaths_graph *graph) {
    int r;
    r = smaths_filter_init(&integrator->filter, graph, (gln_process_fp_t) smaths_integrator_process, integrator);
    if(r != 0) {
	return r;
    }
    memset(&integrator->intg_matrix, 0, sizeof(struct smaths_intg_matrix));

    return 0;
}

void smaths_integrator_destroy(struct smaths_integrator *integrator) {
    smaths_filter_destroy(&integrator->filter);
}
