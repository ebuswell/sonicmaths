/*
 * jbridge.c
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
#include <jack/jack.h>
#include <pthread.h>
#include <graphline.h>
#include <sonicmaths/jbridge.h>
#include <sonicmaths/graph.h>
#include <stdint.h>
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* called via graphline */
static int smaths_jbridge_process_from_jack(struct smaths_jbridge *self) {
    struct smaths_jbridge_socketpair *s;
    atomic_iterator_t i;
    int r;
    r = atomic_iterator_init(&self->graph.from_outside_sockets, &i);
    if(r != 0) {
	return r;
    }
    while((s = atomic_iterator_next(&self->graph.from_outside_sockets, &i)) != ALST_EMPTY) {
	void *in_buffer = jack_port_get_buffer(s->port, self->graph.graph.buffer_nmemb);
	if(in_buffer == NULL) {
	    atomic_iterator_destroy(&self->graph.from_outside_sockets, &i);
	    return -1;
	}
	void *out_buffer = gln_socket_get_buffer(&s->socket);	
	if(out_buffer == NULL) {
	    atomic_iterator_destroy(&self->graph.from_outside_sockets, &i);
	    return -1;
	}
	memcpy(out_buffer, in_buffer, self->graph.graph.buffer_size);
    }

    atomic_iterator_destroy(&self->graph.from_outside_sockets, &i);
    return 0;
}

/* called via jack */
static int smaths_jbridge_process_to_jack(jack_nframes_t nframes, struct smaths_jbridge *self) {
    if(((size_t) nframes) != self->graph.graph.buffer_nmemb) {
	self->graph.graph.buffer_nmemb = (size_t) nframes;
	self->graph.graph.buffer_size = ((size_t) nframes) * sizeof(float);
	void *buffer;
	while((buffer = g_trash_stack_pop(&self->graph.graph.buffer_heap)) != NULL) {
	    free(buffer);
	}
    }

    /* prefill all untied ports */
    struct smaths_jbridge_untied_port *p;
    atomic_iterator_t i;
    int r;
    r = atomic_iterator_init(&self->untied_ports, &i);
    if(r != 0) {
	return r;
    }
    while((p = atomic_iterator_next(&self->untied_ports, &i)) != ALST_EMPTY) {
	r = p->cb(nframes, p->arg);
	if(r != 0) {
	    atomic_iterator_destroy(&self->untied_ports, &i);
	    return r;
	}
    }
    atomic_iterator_destroy(&self->untied_ports, &i);


    r = smaths_graph_process(&self->graph);

    struct smaths_jbridge_socketpair *s;
    r = atomic_iterator_init(&self->graph.to_outside_sockets, &i);
    if(r != 0) {
	return r;
    }
    while((s = atomic_iterator_next(&self->graph.to_outside_sockets, &i)) != ALST_EMPTY) {
	void *out_buffer = jack_port_get_buffer(s->port, nframes);
	if(out_buffer == NULL) {
	    atomic_iterator_destroy(&self->graph.to_outside_sockets, &i);
	    return -1;
	}
	void *in_buffer = gln_socket_get_buffer(&s->socket);	
	if(in_buffer == NULL) {
	    atomic_iterator_destroy(&self->graph.to_outside_sockets, &i);
	    return -1;
	}
	memcpy(out_buffer, in_buffer, self->graph.graph.buffer_size);
    }

    atomic_iterator_destroy(&self->graph.from_outside_sockets, &i);
    gln_graph_reset(&self->graph.graph);
    return 0;
}

int smaths_jbridge_init(struct smaths_jbridge *self, const char *client_name, jack_options_t flags, jack_status_t *status, char *server_name) {
    *status = JackFailure;
    atomic_set(&self->portnum, 0);

    if(server_name == NULL) {
	self->client = jack_client_open(client_name, flags, status);
    } else {
	self->client = jack_client_open(client_name, flags, status, server_name);
    }
    if(self->client == NULL) {
	return -1;
    }

    int r;
    r = smaths_graph_init(&self->graph, (gln_process_fp_t) smaths_jbridge_process_from_jack, self, (size_t) jack_get_buffer_size(self->client), (float) jack_get_sample_rate(self->client));
    if(r != 0) {
	jack_client_close(self->client);
	return r;
    }

    r = jack_set_process_callback(self->client, (JackProcessCallback) smaths_jbridge_process_to_jack, self);
    if(r != 0) {
	jack_client_close(self->client);
	smaths_graph_destroy(&self->graph);
	return r;
    }

    r = atomic_list_init(&self->untied_ports);
    if(r != 0) {
	jack_client_close(self->client);
	smaths_graph_destroy(&self->graph);
	return r;
    }

    r = jack_activate(self->client);
    if(r != 0) {
	atomic_list_destroy(&self->untied_ports);
	jack_client_close(self->client);
	smaths_graph_destroy(&self->graph);
	return r;
    }

    return 0;
}

int smaths_jbridge_destroy(struct smaths_jbridge *self) {
    int r = jack_client_close(self->client);
    smaths_graph_destroy(&self->graph);
    atomic_list_destroy(&self->untied_ports);
    return r;
}

int smaths_jbridge_create_socket(struct smaths_jbridge *self, enum gln_socket_direction direction, struct gln_socket **out_socket, jack_port_t **out_port) {
    int r;

    struct smaths_jbridge_socketpair *s = malloc(sizeof(struct smaths_jbridge_socketpair));
    if(s == NULL) {
	return -1;
    }

    if(direction == INPUT) {
        /* from jack */
	char *tmpnam = alloca(24);
	r = snprintf(tmpnam, 24, "%d-in", atomic_inc_return(&self->portnum));
	if(r < 0) {
	    return r;
	}

	s->port = jack_port_register(self->client, tmpnam, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0); 
	if(s->port == NULL) {
	    return -1;
	}

	r = gln_socket_init(&s->socket, &self->graph.node, OUTPUT);
	if(r != 0) {
	    jack_port_unregister(self->client, s->port);
	    return r;
	}

	r = atomic_list_push(&self->graph.from_outside_sockets, s);
	if(r != 0) {
	    gln_socket_destroy(&s->socket);
	    jack_port_unregister(self->client, s->port);
	    return r;
	}
    } else {
        /* to jack */
	char *tmpnam = alloca(25);
	r = snprintf(tmpnam, 25, "%d-out", atomic_inc_return(&self->portnum));
	if(r < 0) {
	    return r;
	}

	s->port = jack_port_register(self->client, tmpnam, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0); 
	if(s->port == NULL) {
	    return -1;
	}

	r = gln_socket_init(&s->socket, &self->graph.node, INPUT);
	if(r != 0) {
	    jack_port_unregister(self->client, s->port);
	    return r;
	}

	r = atomic_list_push(&self->graph.to_outside_sockets, s);
	if(r != 0) {
	    gln_socket_destroy(&s->socket);
	    jack_port_unregister(self->client, s->port);
	    return r;
	}
    }

    *out_port = s->port;
    *out_socket = &s->socket;

    return 0;
}
