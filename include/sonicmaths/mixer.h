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
#ifndef SONICMATHS_MIXER_H
#define SONICMATHS_MIXER_H 1

#include <graphline.h>
#include <sonicmaths/graph.h>
#include <atomickit/atomic-list.h>
#include <sonicmaths/parameter.h>

/**
 * Mixer
 */
struct smaths_mix {
    struct smaths_graph *graph; /** Graph for this synth */
    struct gln_node node;
    struct gln_socket out;
    atomic_list_t in; /** Input parameters */
    atomic_list_t in_amp; /** Amplitude parameters */
};

/**
 * Destroy mixer
 */
void smaths_mix_destroy(struct smaths_mix *mix);

/**
 * Initialize mixer
 */
int smaths_mix_init(struct smaths_mix *mix, struct smaths_graph *graph);

/**
 * Create an input to this mixer
 *
 * Caller is responsible for memory management.
 */
int smaths_mix_input_init(struct smaths_mix *mix, struct smaths_parameter *in, struct smaths_parameter *in_amp);

/**
 * Create an input to this mixer
 *
 * Caller is responsible for memory management.
 */
void smaths_mix_input_destroy(struct smaths_mix *mix, struct smaths_parameter *in, struct smaths_parameter *in_amp);

#endif /* ! SONICMATHS_MIXER_H */
