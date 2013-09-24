/*
 * jbridge.c
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
#include <string.h>
#include <stdio.h>
#include <jack/jack.h>
#include <jack/midiport.h>
#include <atomickit/atomic.h>
#include <atomickit/atomic-float.h>
#include <atomickit/atomic-malloc.h>
#include <atomickit/atomic-rcp.h>
#include <atomickit/atomic-array.h>
#include <graphline.h>
#include "sonicmaths/buffer.h"
#include "sonicmaths/graph.h"
#include "sonicmaths/midi.h"
#include "sonicmaths/jbridge.h"

static int smaths_jbridge_sample_rate_change(jack_nframes_t nframes, struct smaths_jbridge *jbridge) {
    atomic_float_store_explicit(&jbridge->sample_rate, (float) nframes,
				memory_order_release);
    return 0;
}

static int smaths_jbridge_ts_compar(const int *a, const int *b) {
    int aa, bb;
    if(*a == -*b) {
	return *a - *b;
    } else {
	/* compare absolute values */
	aa = *a >> 31;
	bb = *b >> 31;
	aa = (*a ^ aa) - aa;
	bb = (*b ^ bb) - bb;
	return aa - bb;
    }
}

/* called via graphline */
static int smaths_jbridge_process_from_jack(struct smaths_jbridge *jbridge) {
    struct aary *from_outside;
    size_t i, n;
    int nframes;

    nframes = smaths_graph_frames_per_period(jbridge);
    from_outside = (struct aary *) arcp_load(&jbridge->from_outside_sockets);
    n = aary_length(from_outside);

    for(i = 0; i < n; i++) {
	struct smaths_jbridge_socketpair *s;
	float *in_buffer;
	struct smaths_buffer *out_buffer;

	s = (struct smaths_jbridge_socketpair *) arcp_weakref_load((struct arcp_weakref *) aary_load_phantom(from_outside, i));
	if(s == NULL) {
	    continue;
	}
	in_buffer = jack_port_get_buffer(s->port, nframes);
	if(in_buffer == NULL) {
	    arcp_release(from_outside);
	    arcp_release(s);
	    return -1;
	}
	if(strcmp(jack_port_type(s->port), JACK_DEFAULT_MIDI_TYPE) == 0) {
	    int nevents = jack_midi_get_event_count(in_buffer);
	    int nchannels = 1;
	    jack_midi_event_t event;
	    int j;
	    {
		int event_overlap_array[nevents * 2];
		for(j = 0; j < nevents; j++) {
		    jack_midi_event_get(&event, in_buffer, j);
		    event_overlap_array[j*2] = (int) event.time;
		    event_overlap_array[j*2 + 1] = -(((int) event.time) + 1 + (((int) event.size) - 1) / 4); /* ceil division */
		}
		qsort(event_overlap_array, nevents * 2, sizeof(int), (int (*)(const void *, const void *)) smaths_jbridge_ts_compar);
		int c_nchannels = 0;
		for(j = 0; j < nevents * 2; j++) {
		    if(event_overlap_array[j] < 0) {
			c_nchannels--;
		    } else {
			c_nchannels++;
		    }
		    if(c_nchannels > nchannels) {
			nchannels = c_nchannels;
		    }
		}
	    }
	    out_buffer = smaths_alloc_buffer(s, nframes, nchannels);
	    if(out_buffer == NULL) {
		arcp_release(s);
		arcp_release(from_outside);
		return -1;
	    }
	    int frame_array[nchannels];
	    struct smaths_midi_buffer *midi_buffer = (struct smaths_midi_buffer *) out_buffer;
	    int k;
	    memset(frame_array, 0, sizeof(int) * nchannels);
	    memset(midi_buffer->data, 0, nframes * nchannels * 4);
	    for(j = 0; j < nevents; j++) {
		jack_midi_event_get(&event, in_buffer, j);
		for(k = 0; frame_array[k] > (int) event.time; k++);
		frame_array[k] = (((int) event.time) + 1 + (((int) event.size) - 1) / 4);
		int offset = (event.time * nchannels + k)*4;
		do {
		    switch(event.size % 4) {
		    case 0:
			midi_buffer->data[offset++] = *event.buffer++;
			event.size--;
		    case 3:
			midi_buffer->data[offset++] = *event.buffer++;
			event.size--;
		    case 2:
			midi_buffer->data[offset++] = *event.buffer++;
			event.size--;
		    case 1:
			midi_buffer->data[offset++] = *event.buffer++;
			event.size--;
		    }
		    offset += nchannels * 4;
		} while(event.size > 0);
	    }
	} else {
	    out_buffer = smaths_alloc_buffer(s, nframes, 1);
	    if(out_buffer == NULL) {
		arcp_release(s);
		arcp_release(from_outside);
		return -1;
	    }
	    memcpy(out_buffer->data, in_buffer, sizeof(float) * nframes);
	}
	arcp_release(s);
    }

    arcp_release(from_outside);
    return 0;
}

/* called via jack */
static int smaths_jbridge_process_to_jack(jack_nframes_t nframes, struct smaths_jbridge *jbridge) {
    int r;
    size_t i, j, n;
    struct aary *to_outside;
    struct smaths_buffer **buffers;
    struct smaths_jbridge_socketpair **socketpairs;

    atomic_store_explicit(&jbridge->frames_per_period, nframes, memory_order_release);

    r = smaths_graph_process(jbridge);
    if(r != 0) {
	return r;
    }

    to_outside = (struct aary *) arcp_load(&jbridge->to_outside_sockets);
    n = aary_length(to_outside);

    buffers = alloca(n * sizeof(struct smaths_buffer *));
    socketpairs = alloca(n * sizeof(struct smaths_jbridge_socketpair *));

    for(i = 0, j = 0; i < n; i++) {
	struct smaths_jbridge_socketpair *s;
	s = (struct smaths_jbridge_socketpair *) arcp_weakref_load((struct arcp_weakref *) aary_load_phantom(to_outside, i));
	if(s == NULL) {
	    continue;
	}
	socketpairs[j++] = s;
    }
    n = j;
    i = 0;
    arcp_release(to_outside);

    r = gln_get_buffer_list(n, (struct gln_socket **) socketpairs, (void **) buffers);
    if(r != 0) {
	goto abort;
    }

    for(; i < n; i++) {
	struct smaths_jbridge_socketpair *s;
	float *out_buffer;
	s = socketpairs[i];
	out_buffer = jack_port_get_buffer(s->port, nframes);
	if(out_buffer == NULL) {
	    r = -1;
	    goto abort;
	}
	if(buffers[i] == NULL) {
	    memset(out_buffer, 0, sizeof(float) * nframes);
	    continue;
	}
	if(buffers[i]->nchannels == 1) {
	    memcpy(out_buffer, buffers[i]->data, sizeof(float) * buffers[i]->nframes);
	} else {
	    int k;
	    int l;
	    for(k = 0; k < buffers[i]->nframes; k++) {
		out_buffer[k] = 0.0f;
		for(l = 0; l < buffers[i]->nchannels; l++) {
		    out_buffer[k] += buffers[i]->data[k * buffers[i]->nchannels + l];
		}
	    }
	}
	arcp_release(s);
    }

    gln_graph_reset(jbridge);
    return 0;

abort:
    for(; i < n; i++) {
	arcp_release(socketpairs[i]);
    }
    return r;
}

int smaths_jbridge_init(struct smaths_jbridge *jbridge, const char *client_name, jack_options_t flags, jack_status_t *status, char *server_name, void (*destroy)(struct smaths_jbridge *)) {
    int r = -1;

    *status = JackFailure;
    atomic_init(&jbridge->portnum, 0);

    if(server_name == NULL) {
	jbridge->client = jack_client_open(client_name, flags, status);
    } else {
	jbridge->client = jack_client_open(client_name, flags, status, server_name);
    }
    if(jbridge->client == NULL) {
	goto undo0;
    }

    r = smaths_graph_init(jbridge, (smaths_graph_process_fp_t) smaths_jbridge_process_from_jack, (void (*)(struct smaths_graph *)) destroy);
    if(r != 0) {
	goto undo1;
    }

    atomic_store_explicit(&jbridge->frames_per_period, jack_get_buffer_size(jbridge->client), memory_order_release);
    atomic_float_store_explicit(&jbridge->sample_rate, (float) jack_get_sample_rate(jbridge->client), memory_order_release);

    r = jack_set_sample_rate_callback(jbridge->client, (JackSampleRateCallback) smaths_jbridge_sample_rate_change, jbridge);
    if(r != 0) {
	goto undo2;
    }

    r = jack_set_process_callback(jbridge->client, (JackProcessCallback) smaths_jbridge_process_to_jack, jbridge);
    if(r != 0) {
	goto undo2;
    }

    r = jack_activate(jbridge->client);
    if(r != 0) {
	goto undo2;
    }

    return 0;

undo2:
    smaths_graph_destroy(jbridge);
undo1:
    jack_client_close(jbridge->client);
undo0:
    return r;
}

void smaths_jbridge_destroy(struct smaths_jbridge *jbridge) {
    int r;
    r = jack_client_close(jbridge->client);
    if(r != 0) {
	perror("Failed to close jack_client");
    }
    smaths_graph_destroy(jbridge);
}

static void smaths_jbridge_socketpair_destroy(struct smaths_jbridge_socketpair *s) {
    struct gln_node *jb_node;
    struct smaths_jbridge *jb;
    jb_node = (struct gln_node *) arcp_weakref_load(s->node);
    if(jb_node != NULL) {
	jb = (struct smaths_jbridge *) arcp_weakref_load(jb_node->graph);
	if(jb != NULL) {
	    struct aary *splist;
	    struct aary *new_splist;
	    jack_port_unregister(jb->client, s->port);
	    if(s->direction == GLNS_OUTPUT) {
		do {
		    splist = (struct aary *) arcp_load(&jb->from_outside_sockets);
		    new_splist = aary_dup_set_remove(splist, arcp_weakref_phantom(s));
		    if(new_splist == NULL) {
			arcp_release(splist);
			break;
		    }
		} while(!arcp_compare_store_release(&jb->from_outside_sockets, splist, new_splist));
	    } else {
		do {
		    splist = (struct aary *) arcp_load(&jb->to_outside_sockets);
		    new_splist = aary_dup_set_remove(splist, arcp_weakref_phantom(s));
		    if(new_splist == NULL) {
			arcp_release(splist);
			break;
		    }
		} while(!arcp_compare_store_release(&jb->to_outside_sockets, splist, new_splist));
	    }
	    arcp_release(jb);
	}
	arcp_release(jb_node);
    }
    gln_socket_destroy(s);
    afree(s, sizeof(struct smaths_jbridge_socketpair));
}

static int smaths_jbridge_create_socket_type(struct smaths_jbridge *jbridge, enum gln_socket_direction direction, const char *port_type, struct gln_socket **socket, jack_port_t **port) {
    int r = -1;
    struct smaths_jbridge_socketpair *s;

    s = amalloc(sizeof(struct smaths_jbridge_socketpair));
    if(s == NULL) {
	goto undo0;
    }

    if(direction == GLNS_INPUT) {
        /* from jack */
	char *tmpnam = alloca(24);
	r = snprintf(tmpnam, 24, "%d-in", atomic_fetch_add_explicit(&jbridge->portnum, 1, memory_order_acq_rel));
	if(r < 0) {
	    goto undo1;
	}

	s->port = jack_port_register(jbridge->client, tmpnam, port_type, JackPortIsInput, 0); 
	if(s->port == NULL) {
	    goto undo1;
	}

	r = gln_socket_init(s, jbridge->node, GLNS_OUTPUT, (void (*)(struct gln_socket *)) smaths_jbridge_socketpair_destroy);
	if(r != 0) {
	    goto undo2;
	}

	struct aary *from_outside;
	struct aary *new_from_outside;
	do {
	    struct arcp_weakref *weak_s = arcp_weakref(s);
	    from_outside = (struct aary *) arcp_load(&jbridge->from_outside_sockets);
	    new_from_outside = aary_dup_set_add(from_outside, weak_s);
	    if(new_from_outside == NULL) {
		arcp_release(from_outside);
		arcp_release(weak_s);
		r = -1;
		goto undo3;
	    }
	} while(!arcp_compare_store_release(&jbridge->from_outside_sockets, from_outside, new_from_outside));
    } else {
        /* to jack */
	char *tmpnam = alloca(25);
	r = snprintf(tmpnam, 25, "%d-out", atomic_fetch_add_explicit(&jbridge->portnum, 1, memory_order_acq_rel));
	if(r < 0) {
	    goto undo1;
	}

	s->port = jack_port_register(jbridge->client, tmpnam, port_type, JackPortIsOutput, 0); 
	if(s->port == NULL) {
	    goto undo1;
	}

	r = gln_socket_init(s, jbridge->node, GLNS_INPUT, (void (*)(struct gln_socket *)) smaths_jbridge_socketpair_destroy);
	if(r != 0) {
	    goto undo2;
	}

	struct aary *to_outside;
	struct aary *new_to_outside;
	do {
	    struct arcp_weakref *weak_s = arcp_weakref(s);
	    to_outside = (struct aary *) arcp_load(&jbridge->to_outside_sockets);
	    new_to_outside = aary_dup_set_add(to_outside, weak_s);
	    if(new_to_outside == NULL) {
		arcp_release(weak_s);
		arcp_release(to_outside);
		r = -1;
		goto undo3;
	    }
	} while(!arcp_compare_store_release(&jbridge->to_outside_sockets, to_outside, new_to_outside));
    }

    *port = s->port;
    *socket = s;

    return 0;

undo3:
    gln_socket_destroy(s);
undo2:
    jack_port_unregister(jbridge->client, s->port);
undo1:
    afree(s, sizeof(struct smaths_jbridge_socketpair));
undo0:
    return r;
}

int smaths_jbridge_create_socket(struct smaths_jbridge *jbridge, enum gln_socket_direction direction, struct gln_socket **socket, jack_port_t **port) {
    return smaths_jbridge_create_socket_type(jbridge, direction, JACK_DEFAULT_AUDIO_TYPE, socket, port);
}

int smaths_jbridge_create_midi_socket(struct smaths_jbridge *jbridge, enum gln_socket_direction direction, struct gln_socket **socket, jack_port_t **port) {
    return smaths_jbridge_create_socket_type(jbridge, direction, JACK_DEFAULT_MIDI_TYPE, socket, port);
}

static void __smaths_jbridge_destroy(struct smaths_jbridge *jbridge) {
    smaths_jbridge_destroy(jbridge);
    afree(jbridge, sizeof(struct smaths_jbridge));
}

struct smaths_jbridge *smaths_jbridge_create(const char *client_name, jack_options_t flags, jack_status_t *status, char *server_name) {
    int r;
    struct smaths_jbridge *jbridge;

    jbridge = amalloc(sizeof(struct smaths_jbridge));
    if(jbridge == NULL) {
	return NULL;
    }

    r = smaths_jbridge_init(jbridge, client_name, flags, status, server_name, __smaths_jbridge_destroy);
    if(r != 0) {
	afree(jbridge, sizeof(struct smaths_jbridge));
	return NULL;
    }

    return jbridge;
}
