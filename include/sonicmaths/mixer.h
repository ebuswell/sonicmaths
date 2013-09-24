/** @file mixer.h
 *
 * Mixer
 *
 * This simple mixer class just adds its arguments together, scaled by
 * their respective amplitudes.  Additionally, it serves as the
 * subclass of all things that mix two things to get one thing.  Right
 * now, that's just Modulator (@ref modulator.h).
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
#ifndef SONICMATHS_MIXER_H
#define SONICMATHS_MIXER_H 1

#include <atomickit/atomic-rcp.h>
#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>

struct smaths_mix_input {
    struct smaths_parameter;
    struct smaths_parameter *in_amp;
};

/**
 * Mixer
 */
struct smaths_mix {
    struct gln_node;
    struct gln_socket *out; /** Output */
    arcp_t in; /** Input parameters */
};

/**
 * Destroy mixer
 */
void smaths_mix_destroy(struct smaths_mix *mix);

/**
 * Initialize mixer
 */
int smaths_mix_init(struct smaths_mix *mix, struct smaths_graph *graph, void (*destroy)(struct smaths_mix *));

struct smaths_mix *smaths_mix_create(struct smaths_graph *graph);

/**
 * Create an input to this mixer
 */
int smaths_mix_input_create(struct smaths_mix *mix, struct smaths_parameter **in, struct smaths_parameter **in_amp);

#endif /* ! SONICMATHS_MIXER_H */
