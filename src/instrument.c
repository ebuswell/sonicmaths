/*
 * instrument.c
 * 
 * Copyright 2011 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 2 of the License,
 * or (at your option) any later version.
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
#include "sonicmaths/instrument.h"
#include "sonicmaths/controller.h"
#include <atomickit/atomic-float.h>
#include <graphline.h>

static int smaths_inst_process(struct smaths_inst *self) {
    float *out_buffer = gln_socket_get_buffer(&self->ctlr.out);
    if(out_buffer == NULL) {
	return -1;
    }
    float *ctl_buffer = gln_socket_get_buffer(&self->ctlr.ctl);
    if(ctl_buffer == NULL) {
	return -1;
    }

    float ctl = atomic_float_xchg(&self->ctl, 0.0f);
    if(ctl != 0.0) {
	ctl_buffer[0] = ctl;
    }
    float value = atomic_float_read(&self->value);
    size_t i;
    for(i = 0; i < self->ctlr.graph->graph.buffer_nmemb; i++) {
	out_buffer[i] = value;
    }
    return 0;
}

int smaths_inst_init(struct smaths_inst *self, struct smaths_graph *graph) {
    int r;
    r = smaths_ctlr_init(&self->ctlr, graph, (gln_process_fp_t) smaths_inst_process, self);
    if(r != 0) {
	return r;
    }
    
    atomic_float_set(&self->value, 0.0f);
    atomic_float_set(&self->ctl, 0.0f);
    return 0;
}

void smaths_inst_destroy(struct smaths_inst *self) {
    smaths_ctlr_destroy(&self->ctlr);
}
