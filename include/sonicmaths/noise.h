/** @file noise.h
 *
 * Noise Generator
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
#ifndef SONICMATHS_NOISE_H
#define SONICMATHS_NOISE_H 1

#include <atomickit/atomic.h>
#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>

enum smaths_noise_kind {
    SMATHSN_GAUSSIAN,
    SMATHSN_UNIFORM
};

/**
 * Noise Generator
 *
 * See @ref struct smaths_synth, although this is not a strict
 * subclass since there's no @p freq input.
 */
struct smaths_noise {
    struct gln_node;
    struct gln_socket *out;
    struct smaths_parameter *amp;
    struct smaths_parameter *offset;
    atomic_int kind; /** gaussian, uniform, tick, */
};

/**
 * Destroy noise generator
 */
void smaths_noise_destroy(struct smaths_noise *noise);

/**
 * Initialize noise generator
 */
int smaths_noise_init(struct smaths_noise *noise, struct smaths_graph *graph, void (*destroy)(struct smaths_noise *));

struct smaths_noise *smaths_noise_create(struct smaths_graph *graph);

static inline void smaths_noise_set_kind(struct smaths_noise *noise, enum smaths_noise_kind kind) {
    atomic_store_explicit(&noise->kind, kind, memory_order_release);
}

static inline enum smaths_noise_kind smaths_noise_get_kind(struct smaths_noise *noise) {
    return atomic_load_explicit(&noise->kind, memory_order_consume);
}

#endif
