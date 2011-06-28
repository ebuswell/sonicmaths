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
#include <math.h>

/**
 * A parameter which may be either static or dynamic
 */
struct smaths_parameter {
    atomic_float_t value;
    struct gln_socket p_static;
    struct gln_socket p_dynamic;
};

int smaths_parameter_init(struct smaths_parameter *p, struct gln_node *node, float value);

void smaths_parameter_destroy(struct smaths_parameter *p);

float *smaths_parameter_get_buffer(struct smaths_parameter *p);

static inline void smaths_parameter_set(struct smaths_parameter *p, float value) {
    atomic_float_set(&p->value, value);
}

static inline int smaths_parameter_connect(struct smaths_parameter *p, struct gln_socket *other) {
    int r = gln_socket_connect(&p->p_dynamic, other);
    if(r != 0) {
	return r;
    }
    atomic_float_set(&p->value, NAN);
    return 0;
}

#endif /* ! SONICMATHS_PARAMETER_H */
