/** @file envelope_generator.h
 *
 * Envelope Generator
 *
 * Creates an envelope corresponding to a control signal.
 *
 * @verbatim
           _ attack_a
        _-- \
      --     \
    /          \________ sustain_a
   /                    \
 /                       \
/                          \
release_a                   release_a
|__________|___|       |___|
 attack_t   decay_t     release_t
@endverbatim
 *
 * The bad ascii art is trying to illustrate the default exponential version.
 *
 * If you want the addition of "hold" time, run the output through a distortion filter.
 * That's likely to give you a more releastic punch then adding in a hold parameter
 * anyway.
 *
 * Also, note that by setting the appropriate parameters you can invert the envelope or do
 * many other nonstandard things useful in controlling filters.  The predictable way that
 * a linear envelope interacts with lin2exp (@ref lin2exp.h) is important for certain effects.
 *
 * The attack and decay cycle are always performed.  A control signal to release during
 * this time will cause the release to happen immediately after the decay.  Conversely, a
 * control signal to attack starts the attack cycle immediately.
 *
 * As a last warning, I suspect the existence of bugs I have not yet been able to identify
 * when attack_a and release_a are not the default values.
 *
 * @todo parameterize a forced release.
 */
/*
 * Copyright 2011 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 2 of the License,
 * or (at your option) any later version.
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
#ifndef SONICMATHS_ENVELOPE_GENERATOR_H
#define SONICMATHS_ENVELOPE_GENERATOR_H 1

#include <stdbool.h>
#include <atomickit/atomic-types.h>
#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>

/**
 * Envelope Generator State
 */
enum smaths_envg_state {
    ATTACK,
    DECAY,
    SUSTAIN,
    RELEASE,
    FINISHED
};

/**
 * Envelope Generator
 */
struct smaths_envg {
    struct smaths_graph *graph;
    struct gln_node node;
    struct gln_socket ctl; /** Input control data */
    struct gln_socket out; /** Output */
    struct smaths_parameter attack_t; /** Attack time */
    struct smaths_parameter attack_a; /** Attack amplitude */
    struct smaths_parameter decay_t; /** Decay time */
    struct smaths_parameter sustain_a; /** Sustain amplitude */
    struct smaths_parameter release_t; /** Release time */
    struct smaths_parameter release_a; /** Release amplitude */
    atomic_t linear; /** Whether it's linear or exponential */

    enum smaths_envg_state state; /** Keeps track of the state */
    float last_a; /** The last value we saw */
    bool release; /** Whether we should release when it's apropos */
    bool upwards; /** Keeps track of which direction we're going */
};

/**
 * Destroy envelope generator
 */
void smaths_envg_destroy(struct smaths_envg *envg);

/**
 * Initialize envelope generator
 */
int smaths_envg_init(struct smaths_envg *envg, struct smaths_graph *graph);

#endif
