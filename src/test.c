/*
 * test.c
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
#include <stdio.h>
#include <error.h>
#include <sonicmaths/jbridge.h>
#include <sonicmaths/sine.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>
#include <graphline.h>
#include <string.h>
#include <errno.h>

#define CHECKING(function)			\
    printf("Checking " #function "...")

#define CHECKING_S(string)			\
    printf("Checking " string "...")

#define OK()					\
    printf("OK\n")

#define CHECK_NULL(obj)				\
    do {					\
	if(obj == NULL) {			\
	    error(1, errno, "Error");		\
	}					\
    } while(0)

#define CHECK_R()				\
    do {					\
	if(r != 0) {				\
	    error(1, errno, "Error");		\
	}					\
    } while(0)

/* struct nullnode { */
/*     struct gln_node node; */
/*     struct gln_socket in; */
/*     struct gln_socket out; */
/* }; */

/* int nullnode_f(struct nullnode *self) { */
/*     void *in_buffer = gln_socket_get_buffer(&self->in); */
/*     if(in_buffer == NULL) */
/* 	return -1; */
/*     void *out_buffer = gln_socket_get_buffer(&self->out); */
/*     if(out_buffer == NULL) */
/* 	return -1; */

/*     memcpy(out_buffer, in_buffer, self->node.graph->buffer_size); */

/*     return 0; */
/* } */

int main(int argc __attribute__((unused)), char **argv __attribute__((unused))) {
    int r;

    jack_status_t status;
    CHECKING(smaths_jbridge_init);
    struct smaths_jbridge bridge;
    r = smaths_jbridge_init(&bridge, "smaths_jbridge", 0, &status, NULL);
    if(r != 0) {
	if(status & JackServerFailed) {
	    error(0, 0, "Unable to connect to the JACK server");
	}
	if(status & JackServerError) {
	    error(0, 0, "Communication error with the JACK server");
	}
	if(status & JackNoSuchClient) {
	    error(0, 0, "Requested client does not exist");
	}
	if(status & JackLoadFailure) {
	    error(0, 0, "Unable to load internal client");
	}
	if(status & JackInitFailure) {
	    error(0, 0, "Unable to initialize client");
	}
	if(status & JackShmFailure) {
	    error(0, 0, "Unable to access shared memory");
	}
	if(status & JackVersionError) {
	    error(0, 0, "Client's protocol version does not match");
	}
	if(status & JackBackendError) {
	    error(0, 0, "Backend error");
	}
	if(status & JackClientZombie) {
	    error(0, 0, "Client zombie");
	}
	if(status & JackInvalidOption) {
	    error(0, 0, "The operation contained an invalid or unsupported option");
	}
	if(status & JackNameNotUnique) {
	    error(0, 0, "The desired client name was not unique");
	}
	error(1, 0, "Overall operation failed: %d", status);
    }
    OK();

    CHECKING(smaths_jbridge_create_socket);
    struct gln_socket *to_jack_socket;
    jack_port_t *to_jack_port;
    r = smaths_jbridge_create_socket(&bridge, OUTPUT, &to_jack_socket, &to_jack_port);
    CHECK_R();

    /* struct gln_socket *from_jack_socket; */
    /* jack_port_t *from_jack_port; */
    /* r = smaths_jbridge_create_socket(&bridge, INPUT, &from_jack_socket, &from_jack_port); */
    /* CHECK_R(); */

    r = jack_connect(bridge.client, jack_port_name(to_jack_port), "system:playback_1");
    CHECK_R();
    r = jack_connect(bridge.client, jack_port_name(to_jack_port), "system:playback_2");
    CHECK_R();
    OK();

    /* CHECKING_S("nullnode creation and hook up"); */
    /* struct nullnode nn; */
    /* r = gln_node_init(&nn.node, &bridge.graph.graph, (gln_process_fp_t) nullnode_f, &nn); */
    /* CHECK_R(); */
    /* r = gln_socket_init(&nn.in, &nn.node, INPUT); */
    /* CHECK_R(); */
    /* r = gln_socket_init(&nn.out, &nn.node, OUTPUT); */
    /* CHECK_R(); */
    /* r = gln_socket_connect(&nn.in, from_jack_socket); */
    /* CHECK_R(); */
    /* r = gln_socket_connect(&nn.out, to_jack_socket); */
    /* CHECK_R(); */
    /* OK(); */

    CHECKING(smaths_sine_init());
    struct smaths_sine sine;
    r = smaths_sine_init(&sine, &bridge.graph);
    CHECK_R();
    r = gln_socket_connect(&sine.out, to_jack_socket);
    CHECK_R();
    OK();

    CHECKING_S("smaths_parameter_set");
    smaths_parameter_set(&sine.freq, smaths_graph_normalized_frequency(&bridge.graph, 220.0));
    OK();

    printf("Hit return to quit");
    getchar();

    CHECKING(smaths_jbridge_destroy);
    smaths_sine_destroy(&sine);
    r = smaths_jbridge_destroy(&bridge);
    CHECK_R();
    OK();

    return 0;
}
