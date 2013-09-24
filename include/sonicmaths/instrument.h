/** @file instrument.h
 *
 * Instrument
 *
 * A simple "instrument" interface, to use your quick typing skills to
 * play notes.  Mostly useful to test things out.
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
#ifndef SONICMATHS_INSTRUMENT_H
#define SONICMATHS_INSTRUMENT_H 1

#include <atomickit/atomic-float.h>
#include <graphline.h>
#include <sonicmaths/controller.h>
#include <sonicmaths/graph.h>

/**
 * Instrument
 *
 * See @ref struct smaths_ctlr
 */
struct smaths_inst {
    struct smaths_ctlr;
    atomic_int user_nchannels;
    int nchannels;
    float *out_v;
    int *channels_lru_start;
    int *channels_lru_stop;
    aqueue_t cmd_queue;
};

enum smaths_inst_cmd {
    SMATHSIC_START,
    SMATHSIC_STOP
};

struct smaths_inst_cmd_s {
    struct arcp_region;
    enum smaths_inst_cmd cmd;
    float value;
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
int smaths_inst_init(struct smaths_inst *inst, struct smaths_graph *graph, void (*destroy)(struct smaths_inst *));

struct smaths_inst *smaths_inst_create(struct smaths_graph *graph);

int smaths_inst_cmd(struct smaths_inst *inst, float value, enum smaths_inst_cmd cmd);

/**
 * Play a note
 *
 * @param value the note to play.
 */
static inline int smaths_inst_play(struct smaths_inst *inst, float value) {
    return smaths_inst_cmd(inst, value, SMATHSIC_START);
}

/**
 * Stop playing a note
 */
static inline int smaths_inst_stop(struct smaths_inst *inst, float value) {
    return smaths_inst_cmd(inst, value, SMATHSIC_STOP);
}

static inline int smaths_inst_get_nchannels(struct smaths_inst *inst) {
    return atomic_load_explicit(&inst->user_nchannels, memory_order_acquire);
}

static inline void smaths_inst_set_nchannels(struct smaths_inst *inst, int nchannels) {
    return atomic_store_explicit(&inst->user_nchannels, nchannels, memory_order_release);
}

#endif /* ! SONICMATHS_INSTRUMENT_H */
