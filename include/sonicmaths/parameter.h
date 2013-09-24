/** @file parameter.h
 *
 * Defines a simple, settable parameter
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
#ifndef SONICMATHS_PARAMETER_H
#define SONICMATHS_PARAMETER_H 1

#include <atomickit/atomic-float.h>
#include <graphline.h>
#include <sonicmaths/buffer.h>

/**
 * A parameter which may be either static or dynamic
 */
struct smaths_parameter {
    struct gln_socket;
    atomic_float value;
};

int smaths_parameter_init(struct smaths_parameter *parameter, struct gln_node *node, float value, void (*destroy)(struct smaths_parameter *));

#define smaths_parameter_destroy gln_socket_destroy

struct smaths_parameter *smaths_parameter_create(struct gln_node *node, float value);

static inline void smaths_parameter_set(struct smaths_parameter *parameter, float value) {
    atomic_float_store_explicit(&parameter->value, value, memory_order_release);
}

static inline float smaths_parameter_get(struct smaths_parameter *parameter) {
    return atomic_float_load_explicit(&parameter->value, memory_order_acquire);
}

static inline float smaths_parameter_go(struct smaths_parameter *parameter, struct smaths_buffer *buffer) {
    if(buffer != NULL) {
	float ret = buffer->data[buffer->nchannels * (buffer->nframes - 1)];
	smaths_parameter_set(parameter, ret);
	return ret;
    }
    return smaths_parameter_get(parameter);
}

#endif /* ! SONICMATHS_PARAMETER_H */
