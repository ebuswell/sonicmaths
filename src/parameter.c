/*
 * parameter.c
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

#include <atomickit/atomic-float.h>
#include <atomickit/atomic-malloc.h>
#include <graphline.h>
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"

int smaths_parameter_init(struct smaths_parameter *parameter, struct gln_node *node, float value, void (*destroy)(struct smaths_parameter *)) {
    int r;

    r = gln_socket_init(parameter, node, GLNS_INPUT, (void (*)(struct gln_socket *)) destroy);
    if(r != 0) {
	return r;
    }

    atomic_float_store_explicit(&parameter->value, value, memory_order_release);

    return 0;
}

static void __smaths_parameter_destroy(struct smaths_parameter *parameter) {
    smaths_parameter_destroy(parameter);
    afree(parameter, sizeof(struct smaths_parameter));
}

struct smaths_parameter *smaths_parameter_create(struct gln_node *node, float value) {
    int r;
    struct smaths_parameter *ret;

    ret = amalloc(sizeof(struct smaths_parameter));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_parameter_init(ret, node, value, __smaths_parameter_destroy);
    if(r != 0) {
	afree(ret, sizeof(struct smaths_parameter));
	return NULL;
    }

    return ret;
}
