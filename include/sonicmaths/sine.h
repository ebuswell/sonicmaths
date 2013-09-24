/** @file sine.h
 *
 * Sine Wave Synth
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
#ifndef SONICMATHS_SINE_H
#define SONICMATHS_SINE_H 1

#include <sonicmaths/graph.h>
#include <sonicmaths/synth.h>

/**
 * Sine Wave Synth
 *
 * See @ref struct smaths_synth
 */
struct smaths_sine {
    struct smaths_synth;
};

/**
 * Destroy sine synth
 *
 * See @ref smaths_synth_destroy
 */
#define smaths_sine_destroy smaths_synth_destroy

/**
 * Initialize sine synth
 *
 * See @ref smaths_synth_destroy
 */
int smaths_sine_init(struct smaths_sine *sine, struct smaths_graph *graph, void (*destroy)(struct smaths_sine *));

struct smaths_sine *smaths_sine_create(struct smaths_graph *graph);

#endif /* ! SONICMATHS_SINE_H */
