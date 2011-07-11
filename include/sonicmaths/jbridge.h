/** @file jbridge.h
 *
 * Bridge to Jack.  Currently, this is the only interface to the
 * outside world.
 *
 */
/*
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
#ifndef SONICMATHS_JBRIDGE_H
#define SONICMATHS_JBRIDGE_H 1

#include <jack/jack.h>
#include <graphline.h>
#include <atomickit/atomic-list.h>
#include <sonicmaths/graph.h>
#include <atomickit/atomic-types.h>

/**
 * Bridge to JACK
 */
struct smaths_jbridge {
    struct smaths_graph graph;
    jack_client_t *client;
    atomic_t portnum;
    atomic_list_t untied_ports;
};

struct smaths_jbridge_socketpair {
    struct gln_socket socket;
    jack_port_t *port;
};

struct smaths_jbridge_untied_port {
    JackProcessCallback cb;
    void *arg;
};

/**
 * Destroy JACK Bridge
 *
 * @returns @c 0 on success, nonzero otherwise.
 */
int smaths_jbridge_destroy(struct smaths_jbridge *jbridge);

/**
 * Initialize JACK Bridge
 *
 * @p client_name, @p flags, @p status, and @p server_name are passed
 * directly to JACK.
 *
 * @returns @c 0 on success, nonzero otherwise.
 */
int smaths_jbridge_init(struct smaths_jbridge *jbridge, const char *client_name, jack_options_t flags, jack_status_t *status, char *server_name);

/**
 * Creates a @ref gln_socket and a corresponding @c jack_port_t
 *
 * @p direction indicates whether the socket can be used to write to
 * JACK (@c OUTPUT) or read from JACK (@c INPUT).  Note that the
 * socket which is created to correspond with the JACK port is always
 * of the opposite type.
 *
 * @p socket and @p port, if not null, will be filled with pointers to
 * the created socket and port.
 *
 * @returns @c 0 on success, nonzero otherwise
 */
int smaths_jbridge_create_socket(struct smaths_jbridge *jbridge, enum gln_socket_direction direction, struct gln_socket **out_socket, jack_port_t **out_port);

#endif
