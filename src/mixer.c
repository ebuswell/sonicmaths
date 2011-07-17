/*
 * mixer.c
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

#include <sonicmaths/mixer.h>
#include <sonicmaths/parameter.h>
#include <sonicmaths/graph.h>
#include <graphline.h>
#include <atomickit/atomic-list.h>
#include <alloca.h>

static int smaths_mix_process(struct smaths_mix *self) {
    size_t num_in;
    size_t i;
    size_t j;

    float *out_buffer = gln_socket_get_buffer(&self->out);
    if(out_buffer == NULL) {
	return -1;
    }

    atomic_list_readlock(&self->in);
    atomic_list_readlock(&self->in_amp);
    num_in = nonatomic_list_length(&self->in);
    float **in_buffers = alloca(sizeof(float *) * num_in);
    float **amp_buffers = alloca(sizeof(float *) * num_in);
    for(i = 0; i < num_in; i++) {
	in_buffers[i] = smaths_parameter_get_buffer(nonatomic_list_get(&self->in, i));
	if(in_buffers[i] == NULL) {
	    atomic_list_readunlock(&self->in_amp);
	    atomic_list_readunlock(&self->in);
	}
	amp_buffers[i] = smaths_parameter_get_buffer(nonatomic_list_get(&self->in_amp, i));
	if(amp_buffers[i] == NULL) {
	    atomic_list_readunlock(&self->in_amp);
	    atomic_list_readunlock(&self->in);
	}
    }
    atomic_list_readunlock(&self->in_amp);
    atomic_list_readunlock(&self->in);

    for(i = 0; i < self->graph->graph.buffer_nmemb; i++) {
	float a = 0.0f;
	for(j = 0; j < num_in; j++) {
	    a += in_buffers[j][i] * amp_buffers[j][i];
	}
	out_buffer[i] = a;
    }
    return 0;
}

int smaths_mix_init(struct smaths_mix *self, struct smaths_graph *graph) {
    self->graph = graph;
    int r;
    r = gln_node_init(&self->node, &graph->graph, (gln_process_fp_t) smaths_mix_process, self);
    if(r != 0) {
	return r;
    }

    r = gln_socket_init(&self->out, &self->node, OUTPUT);
    if(r != 0) {
	gln_node_destroy(&self->node);
	return r;
    }

    r = atomic_list_init(&self->in);
    if(r != 0) {
	gln_socket_destroy(&self->out);
	gln_node_destroy(&self->node);
	return r;
    }

    r = atomic_list_init(&self->in_amp);
    if(r != 0) {
	atomic_list_destroy(&self->in);
	gln_socket_destroy(&self->out);
	gln_node_destroy(&self->node);
	return r;
    }

    return 0;
}

int smaths_mix_input_init(struct smaths_mix *self, struct smaths_parameter *in, struct smaths_parameter *in_amp) {
    int r;
    r = smaths_parameter_init(in, &self->node, 0.0f);
    if(r != 0) {
	return r;
    }
    r = smaths_parameter_init(in_amp, &self->node, 1.0f);
    if(r != 0) {
	smaths_parameter_destroy(in);
	return r;
    }
    atomic_list_lock(&self->in);
    atomic_list_lock(&self->in_amp);
    r = nonatomic_list_push(&self->in, in);
    if(r != 0) {
	atomic_list_unlock(&self->in_amp);
	atomic_list_unlock(&self->in);
	smaths_parameter_destroy(in_amp);
	smaths_parameter_destroy(in);
	return r;
    }
    r = nonatomic_list_push(&self->in_amp, in_amp);
    if(r != 0) {
	nonatomic_list_pop(&self->in);
	atomic_list_unlock(&self->in_amp);
	atomic_list_unlock(&self->in);
	smaths_parameter_destroy(in_amp);
	smaths_parameter_destroy(in);
	return r;
    }
    atomic_list_unlock(&self->in_amp);
    atomic_list_unlock(&self->in);
    return 0;
}

void smaths_mix_input_destroy(struct smaths_mix *self, struct smaths_parameter *in, struct smaths_parameter *in_amp) {
    atomic_list_lock(&self->in);
    atomic_list_lock(&self->in_amp);
    nonatomic_list_remove_by_value(&self->in, in);
    nonatomic_list_remove_by_value(&self->in_amp, in_amp);
    atomic_list_unlock(&self->in_amp);
    atomic_list_unlock(&self->in);
    smaths_parameter_destroy(in);
    smaths_parameter_destroy(in_amp);
}

void smaths_mix_destroy(struct smaths_mix *self) {
    atomic_list_lock(&self->in);
    atomic_list_lock(&self->in_amp);
    size_t i;
    for(i = 0; i < nonatomic_list_length(&self->in); i++) {
	smaths_parameter_destroy(nonatomic_list_get(&self->in, i));
	smaths_parameter_destroy(nonatomic_list_get(&self->in_amp, i));
    }
    atomic_list_unlock(&self->in_amp);
    atomic_list_unlock(&self->in);
    atomic_list_destroy(&self->in_amp);
    atomic_list_destroy(&self->in);
    gln_socket_destroy(&self->out);
    gln_node_destroy(&self->node);
}
