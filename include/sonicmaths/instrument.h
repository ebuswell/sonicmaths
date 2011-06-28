/** @file instrument.h
 *
 * Instrument
 *
 * A simple "instrument" interface, to use your quick typing skills to play notes.  Mostly useful to test things out.
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
#ifndef SONICMATHS_INSTRUMENT_H
#define SONICMATHS_INSTRUMENT_H 1

#include <atomickit/atomic-float.h>
#include <sonicmaths/controller.h>
#include <sonicmaths/graph.h>

/**
 * Instrument
 *
 * See @ref struct smaths_ctlr
 */
struct smaths_inst {
    struct smaths_ctlr ctlr;
    atomic_float_t value; /** The current output value */
    atomic_float_t ctl; /** The current control value */
};

/**
 * Destroy instrument
 *
 * See @ref smaths_ctlr_destroy
 */
void smaths_inst_destroy(struct smaths_inst *inst);

/**
 * Initialize instrument
 *
 * See @ref smaths_ctlr_init
 */
int smaths_inst_init(struct smaths_inst *inst, struct smaths_graph *graph);

/**
 * Play a note
 *
 * @param value the note to play.
 */
static inline void smaths_inst_play(struct smaths_inst *self, float value) {
    atomic_float_set(&self->value, value);
    atomic_float_set(&self->ctl, 1.0f);
}

/**
 * Stop playing
 */
static inline void smaths_inst_stop(struct smaths_inst *self) {
    atomic_float_set(&self->ctl, -1.0f);
}

#endif /* ! SONICMATHS_INSTRUMENT_H */
