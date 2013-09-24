/*
 * mixer.c
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

#include <alloca.h>
#include <atomickit/atomic-malloc.h>
#include <atomickit/atomic-rcp.h>
#include <atomickit/atomic-array.h>
#include <graphline.h>
#include "sonicmaths/buffer.h"
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/mixer.h"

static int smaths_mix_process(struct smaths_mix *mix) {
    int r, i, j, k, n;

    int nframes;

    struct smaths_buffer **buffers;
    struct smaths_parameter **sockets;
    float *values;
    struct smaths_buffer *out_buffer;


    {
	struct aary *input_list;
	input_list = (struct aary *) arcp_load(&mix->in);
	n = aary_length(input_list);
	buffers = alloca(n * 2 * sizeof(struct smaths_buffer *));
	sockets = alloca(n * 2 * sizeof(struct smaths_parameter *));

	for(i = 0, j = 0; i < n; i++) {
	    struct smaths_mix_input *input;
	    input = (struct smaths_mix_input *) arcp_weakref_load((struct arcp_weakref *) aary_load_phantom(input_list, i));
	    if(input == NULL) {
		continue;
	    }
	    sockets[j++] = input;
	    sockets[j++] = input->in_amp;
	}
	n = j;
	arcp_release(input_list);
    }

    r = gln_get_buffer_list(n, (struct gln_socket **) sockets, (void **) buffers);
    if(r != 0) {
	goto abort;
    }

    values = alloca(n * sizeof(float));
    for(i = 0; i < n; i++) {
	values[i] = smaths_parameter_go(sockets[i], buffers[i]);
    }

    nframes = smaths_node_frames_per_period(mix);
    if(nframes < 0) {
	r = nframes;
	goto abort;
    }

    out_buffer = smaths_alloc_buffer(mix->out, nframes, 1);
    if(out_buffer == NULL) {
	r = -1;
	goto abort;
    }

    for(i = 0; i < nframes; i++) {
	float out = 0.0f;
	for(k = 0; k < n; k += 2) {
	    if(buffers[k] == NULL) {
		out = values[k] * smaths_value(buffers[k + 1], i, 1, values[k + 1]);
	    } else {
		for(j = 0; j < buffers[k]->nchannels; j++) {
		    out += smaths_value(buffers[k], i, j, values[k])
			* smaths_value(buffers[k + 1], i, j, values[k + 1]);
		}
	    }
	}
	out_buffer->data[i] = out;
    }

abort:
    for(i = 0; i < n; i += 2) {
	arcp_release(sockets[i]);
    }
    return r;
}

int smaths_mix_init(struct smaths_mix *mix, struct smaths_graph *graph, void (*destroy)(struct smaths_mix *)) {
    int r = -1;

    struct aary *empty_array = aary_create(0);
    if(empty_array == 0) {
	goto undo0;
    }
    arcp_init(&mix->in, empty_array);

    r = gln_node_init(mix, graph, (gln_process_fp_t) smaths_mix_process, (void (*)(struct gln_node *)) destroy);
    if(r != 0) {
	goto undo1;
    }

    mix->out = gln_socket_create(mix, GLNS_OUTPUT);
    if(mix->out == NULL) {
	goto undo2;
    }

    return r;

undo2:
    gln_node_destroy(mix);
undo1:
    arcp_store(&mix->in, NULL);
undo0:
    return r;
}

void smaths_mix_destroy(struct smaths_mix *mix) {
    arcp_release(mix->out);
    gln_node_destroy(mix);
    arcp_store(&mix->in, NULL);
}

static void smaths_mix_input_destroy(struct smaths_mix_input *input) {
    struct smaths_mix *mix;
    mix = (struct smaths_mix *) arcp_weakref_load(input->node);
    if(mix != NULL) {
	struct aary *input_list;
	struct aary *new_input_list;
	do {
	    input_list = (struct aary *) arcp_load(&mix->in);
	    new_input_list = aary_dup_set_remove(input_list, arcp_weakref_phantom(input));
	    if(new_input_list == NULL) {
		arcp_release(input_list);
		break;
	    }
	} while(!arcp_compare_store_release(&mix->in, input_list, new_input_list));
	arcp_release(mix);
    }
    arcp_release(input->in_amp);
    smaths_parameter_destroy(input);
    afree(input, sizeof(struct smaths_mix_input));
}

int smaths_mix_input_create(struct smaths_mix *mix, struct smaths_parameter **in, struct smaths_parameter **in_amp) {
    int r = -1;

    struct smaths_mix_input *input = amalloc(sizeof(struct smaths_mix_input));
    if(input == NULL) {
	goto undo0;
    }

    input->in_amp = smaths_parameter_create(mix, 1.0f);
    if(input->in_amp == NULL) {
	goto undo1;
    }

    r = smaths_parameter_init(input, mix, 0.0f, (void (*)(struct smaths_parameter *)) smaths_mix_input_destroy);
    if(r != 0) {
	goto undo2;
    }

    struct aary *input_list;
    struct aary *new_input_list;
    do {
	struct arcp_weakref *weak_input = arcp_weakref(input);
	input_list = (struct aary *) arcp_load(&mix->in);
	new_input_list = aary_dup_set_add(input_list, weak_input);
	if(new_input_list == NULL) {
	    arcp_release(weak_input);
	    arcp_release(input_list);
	    goto undo3;
	}
    } while(!arcp_compare_store_release(&mix->in, input_list, new_input_list));

    *in = input;
    *in_amp = (struct smaths_parameter *) arcp_acquire(input->in_amp);

    return 0;

undo3:
    smaths_parameter_destroy(input);
undo2:
    arcp_release(input->in_amp);
undo1:
    afree(input, sizeof(struct smaths_mix_input));
undo0:
    return r;
}

static void __smaths_mix_destroy(struct smaths_mix *mix) {
    smaths_mix_destroy(mix);
    afree(mix, sizeof(struct smaths_mix));
}

struct smaths_mix *smaths_mix_create(struct smaths_graph *graph) {
    int r;
    struct smaths_mix *ret;

    ret = amalloc(sizeof(struct smaths_mix));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_mix_init(ret, graph, __smaths_mix_destroy);
    if(r != 0) {
	afree(ret, sizeof(struct smaths_mix));
	return NULL;
    }

    return ret;
}
