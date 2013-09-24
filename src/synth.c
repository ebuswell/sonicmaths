/*
 * synth.c
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
#include <atomickit/atomic-malloc.h>
#include <graphline.h>
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/synth.h"
#include "mtrand.h"

int smaths_synth_init(struct smaths_synth *synth, struct smaths_graph *graph, gln_process_fp_t func, void (*destroy)(struct smaths_synth *)) {
    int r = -1;

    synth->t = amalloc(sizeof(float));
    if(synth->t == NULL) {
	goto undo0;
    }
    synth->t[0] = 0.0f;
    synth->nchannels = 1;

    r = gln_node_init(synth, graph, func, (void (*)(struct gln_node *)) destroy);
    if(r != 0) {
	goto undo1;
    }

    synth->out = gln_socket_create(synth, GLNS_OUTPUT);
    if(synth->out == NULL) {
	r = -1;
	goto undo2;
    }

    synth->freq = smaths_parameter_create(synth, 0.0f);
    if(synth->freq == NULL) {
	r = -1;
	goto undo3;
    }

    synth->phase = smaths_parameter_create(synth, mt_rand_float());
    if(synth->phase == NULL) {
	r = -1;
	goto undo4;
    }

    synth->amp = smaths_parameter_create(synth, 1.0f);
    if(synth->amp == NULL) {
	r = -1;
	goto undo5;
    }

    synth->offset = smaths_parameter_create(synth, 0.0f);
    if(r != 0) {
	r = -1;
	goto undo6;
    }

    return 0;

undo6:
    arcp_release(synth->amp);
undo5:
    arcp_release(synth->phase);
undo4:
    arcp_release(synth->freq);
undo3:
    arcp_release(synth->out);
undo2:
    gln_node_destroy(synth);
undo1:
    afree(synth->t, sizeof(float));
undo0:
    return r;
}

void smaths_synth_destroy(struct smaths_synth *synth) {
    arcp_release(synth->offset);
    arcp_release(synth->amp);
    arcp_release(synth->phase);
    arcp_release(synth->freq);
    arcp_release(synth->out);
    gln_node_destroy(synth);
    afree(synth->t, sizeof(float) * synth->nchannels);
}
