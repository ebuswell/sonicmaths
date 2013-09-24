/*
 * midi.h
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
#ifndef SONICMATHS_MIDI_H
#define SONICMATHS_MIDI_H 1

#include <stdint.h>
#include <atomickit/atomic.h>
#include <atomickit/atomic-rcp.h>
#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/controller.h>

#define SMATHSM_MAX_INLINE_SIZE sizeof(uint8_t *);

struct smaths_midi_buffer {
    int nframes;
    int nchannels;
    uint8_t data[]; /* 4 bytes per frame */
};

struct smaths_midi_voice {
    float attack_velocity;
    float release_velocity;
    float pressure;
    float note;
};

struct smaths_midi_parameter {
    struct gln_socket;
    uint8_t status;
    uint8_t control;
    uint16_t parameter;
    float value;
    uint16_t value_bits;
    atomic_float min;
    atomic_float max;
};

struct smaths_midi_ctl {
    struct gln_socket;
    uint8_t status;
    uint8_t control;
    uint16_t parameter;
};

struct smaths_midi_switch {
    struct gln_socket;
    uint8_t status;
    uint8_t control;
    uint16_t parameter;
    bool state;
    atomic_float on_value;
    atomic_float off_value;
};

struct smaths_midi_ctlr {
    struct smaths_ctlr;
    struct gln_socket *midi_in;
    struct gln_socket *attack_velocity;
    atomic_float attack_velocity_min;
    atomic_float attack_velocity_max;
    struct gln_socket *release_velocity;
    atomic_float release_velocity_min;
    atomic_float release_velocity_max;
    struct gln_socket *pressure;
    atomic_float pressure_min;
    atomic_float pressure_max;
    atomic_uint_least8_t midi_channel;
    atomic_int_least8_t key_root;
    atomic_bool chromatic;
    atomic_bool omni_on;
    atomic_bool mono_on;
    bool last_mono_on;
    arcp_t parameters;
    arcp_t switches;
    arcp_t ctls;
    atomic_int user_nvoices;
    uint16_t current_parameter;
    int8_t HRVP;
    int nvoices;
    struct smaths_midi_voice *voices;
    int *voices_lru_start;
    int *voices_lru_stop;
};

int smaths_midi_ctlr_init(struct smaths_midi_ctlr *ctlr, struct smaths_graph *graph, void (*destroy)(struct smaths_midi_ctlr *ctlr));

void smaths_midi_ctlr_destroy(struct smaths_midi_ctlr *ctlr);

struct smaths_midi_ctlr *smaths_midi_ctlr_create(struct smaths_graph *graph);

/* See: http://www.midi.org/techspecs/midimessages.php */

/* Status messages */
#define SMATHSM_CONTROL_CHANGE 0xB0
#define SMATHSM_PROGRAM_CHANGE 0xC0
#define SMATHSM_PRESSURE 0xD0
#define SMATHSM_AFTER_TOUCH 0xD0
#define SMATHSM_PITCH_BEND 0xE0

/* Control Messages */
#define SMATHSM_BANK_SELECT 0x00
#define SMATHSM_MODULATION_WHEEL 0x01
#define SMATHSM_BREATH_CONTROLLER 0x02
/* ? 0x03 */
#define SMATHSM_FOOT_CONTROLLER 0x04
#define SMATHSM_PORTAMENTO_TIME 0x05
/* SMATHSM_DATA_ENTRY 0x06 */
#define SMATHSM_CHANNEL_VOLUME 0x07
#define SMATHSM_MAIN_VOLUME 0x07
#define SMATHSM_BALANCE 0x08
/* ? 0x09 */
#define SMATHSM_PAN 0x0A
#define SMATHSM_EXPRESSION_CONTROLLER 0x0B
#define SMATHSM_EFFECT_CONTROL_1 0x0C
#define SMATHSM_EFFECT_CONTROL_2 0x0D
/* ? 0x0E */
/* ? 0x0F */
#define SMATHSM_GENERAL_PURPOSE_CONTROLLER_1 0x10
#define SMATHSM_GENERAL_PURPOSE_CONTROLLER_2 0x11
#define SMATHSM_GENERAL_PURPOSE_CONTROLLER_3 0x12
#define SMATHSM_GENERAL_PURPOSE_CONTROLLER_4 0x13
/* ? 0x14 */
/* ? 0x15 */
/* ? 0x16 */
/* ? 0x17 */
/* ? 0x18 */
/* ? 0x19 */
/* ? 0x1A */
/* ? 0x1B */
/* ? 0x1C */
/* ? 0x1D */
/* ? 0x1E */
/* ? 0x1F */
/* 0x20-0x3F LSB of 0x00-0x1F */
#define SMATHSM_SUSTAIN_CTL 0x40
#define SMATHSM_HOLD_1_CTL 0x40
#define SMATHSM_DAMPER_PEDAL_CTL 0x40
#define SMATHSM_PORTAMENTO_CTL 0x41
#define SMATHSM_SOSTENUTO_CTL 0x42
#define SMATHSM_SOFT_CTL 0x43
#define SMATHSM_SOFT_PEDAL_CTL 0x43
#define SMATHSM_LEGATO_CTL 0x44
#define SMATHSM_LEGATO_FOOTSWITCH_CTL 0x44
#define SMATHSM_HOLD_2_CTL 0x45
#define SMATHSM_SOUND_VARIATION 0x46
#define SMATHSM_SOUND_CONTROLLER_1 0x46
#define SMATHSM_TIMBRE 0x47
#define SMATHSM_SOUND_CONTROLLER_2 0x47
#define SMATHSM_RELEASE_TIME 0x48
#define SMATHSM_SOUND_CONTROLLER_3 0x48
#define SMATHSM_ATTACK_TIME 0x49
#define SMATHSM_SOUND_CONTROLLER_4 0x49
#define SMATHSM_BRIGHTNESS 0x4A
#define SMATHSM_SOUND_CONTROLLER_5 0x4A
#define SMATHSM_DECAY_TIME 0x4B
#define SMATHSM_SOUND_CONTROLLER_6 0x4B
#define SMATHSM_VIBRATO_RATE 0x4C
#define SMATHSM_SOUND_CONTROLLER_7 0x4C
#define SMATHSM_VIBRATO_DEPTH 0x4D
#define SMATHSM_SOUND_CONTROLLER_8 0x4D
#define SMATHSM_VIBRATO_DELAY 0x4E
#define SMATHSM_SOUND_CONTROLLER_9 0x4E
#define SMATHSM_SOUND_CONTROLLER_10 0x4F
#define SMATHSM_GENERAL_PURPOSE_CONTROLLER_5 0x50
#define SMATHSM_GENERAL_PURPOSE_CONTROLLER_6 0x51
#define SMATHSM_GENERAL_PURPOSE_CONTROLLER_7 0x52
#define SMATHSM_GENERAL_PURPOSE_CONTROLLER_8 0x53
#define SMATHSM_PORTAMENTO_CONTROL 0x54
/* ? 0x55 */
/* ? 0x56 */
/* ? 0x57 */
/* SMATHSM_HIGH_RESOLUTION_VELOCITY_PREFIX 0x58 */
/* ? 0x59 */
/* ? 0x5A */
#define SMATHSM_EFFECTS_1_DEPTH 0x5B
#define SMATHSM_REVERB_SEND_LEVEL 0x5B
#define SMATHSM_EXTERNAL_EFFECTS_DEPTH 0x5B
#define SMATHSM_EFFECTS_2_DEPTH 0x5C
#define SMATHSM_TREMOLO_DEPTH 0x5C
#define SMATHSM_EFFECTS_3_DEPTH 0x5D
#define SMATHSM_CHORUS_SEND_LEVEL 0x5D
#define SMATHSM_CHORUS_DEPTH 0x5D
#define SMATHSM_EFFECTS_4_DEPTH 0x5E
#define SMATHSM_CELESTE_DEPTH 0x5E
#define SMATHSM_CELESTE_DETUNE_DEPTH 0x5E
#define SMATHSM_EFFECTS_5_DEPTH 0x5F
#define SMATHSM_PHASER_DEPTH 0x5F
/* SMATHSM_DATA_INCREMENT 0x60 */
/* SMATHSM_DATA_DECREMENT 0x61 */
/* SMATHSM_NRPN_LSB 0x62 */
/* SMATHSM_NRPN_MSB 0x63 */
/* SMATHSM_RPN_LSB 0x64 */
/* SMATHSM_RPN_MSB 0x65 */
/* ? 0x66 */
/* ? 0x67 */
/* ? 0x68 */
/* ? 0x69 */
/* ? 0x6A */
/* ? 0x6B */
/* ? 0x6C */
/* ? 0x6D */
/* ? 0x6E */
/* ? 0x6F */
/* ? 0x70 */
/* ? 0x71 */
/* ? 0x72 */
/* ? 0x73 */
/* ? 0x74 */
/* ? 0x75 */
/* ? 0x76 */
/* ? 0x77 */

/* Registered parameters */
#define SMATHSM_PITCH_BEND_SENSITIVITY 0x0000
#define SMATHSM_CHANNEL_FINE_TUNING 0x0001
#define SMATHSM_FINE_TUNING 0x0001
#define SMATHSM_CHANNEL_COARSE_TUNING 0x0002
#define SMATHSM_COARSE_TUNING 0x0002
#define SMATHSM_TUNING_PROGRAM_CHANGE 0x0003
#define SMATHSM_TUNING_BANK_SELECT 0x0004
#define SMATHSM_MODULATION_DEPTH_RANGE 0x0005
#define SMATHSM_AZIMUTH_ANGLE 0x3D00
#define SMATHSM_ELEVATION_ANGLE 0x3D01
#define SMATHSM_GAIN 0x3D02
#define SMATHSM_DISTANCE_RATIO 0x3D03
#define SMATHSM_MAXIMUM_DISTANCE 0x3D04
#define SMATHSM_GAIN_AT_MAXIMUM_DISTANCE 0x3D05
#define SMATHSM_REFERENCE_DISTANCE_RATIO 0x3D06
#define SMATHSM_PAN_SPREAD_ANGLE 0x3D07
#define SMATHSM_ROLL_ANGLE 0x3D08

#endif /* ! SONICMATHS_MIDI_H */
