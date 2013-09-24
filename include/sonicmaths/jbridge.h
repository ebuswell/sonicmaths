/** @file jbridge.h
 *
 * Bridge to Jack.  Currently, this is the only interface to the
 * outside world.
 *
 */
/*
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
#ifndef SONICMATHS_JBRIDGE_H
#define SONICMATHS_JBRIDGE_H 1

#include <jack/jack.h>
#include <atomickit/atomic.h>
#include <graphline.h>
#include <sonicmaths/graph.h>

/**
 * Bridge to JACK
 */
struct smaths_jbridge {
    struct smaths_graph;
    jack_client_t *client;
    atomic_int portnum;
};

struct smaths_jbridge_socketpair {
    struct gln_socket;
    jack_port_t *port;
};

/**
 * Destroy JACK Bridge
 *
 * @returns @c 0 on success, nonzero otherwise.
 */
void smaths_jbridge_destroy(struct smaths_jbridge *jbridge);

/**
 * Initialize JACK Bridge
 *
 * @p client_name, @p flags, @p status, and @p server_name are passed
 * directly to JACK.
 *
 * @returns @c 0 on success, nonzero otherwise.
 */
int smaths_jbridge_init(struct smaths_jbridge *jbridge, const char *client_name, jack_options_t flags, jack_status_t *status, char *server_name, void (*destroy)(struct smaths_jbridge *));

struct smaths_jbridge *smaths_jbridge_create(const char *client_name, jack_options_t flags, jack_status_t *status, char *server_name);

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
int smaths_jbridge_create_socket(struct smaths_jbridge *jbridge, enum gln_socket_direction direction, struct gln_socket **socket, jack_port_t **port);

/**
 * Creates a @ref gln_socket and a corresponding @c jack_port_t for
 * communication of midi data
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
int smaths_jbridge_create_midi_socket(struct smaths_jbridge *jbridge, enum gln_socket_direction direction, struct gln_socket **socket, jack_port_t **port);

#endif
