/** @file jmidi.h
 *
 * Midi from Jack.
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
#ifndef SONICMATHS_JMIDI_H
#define SONICMATHS_JMIDI_H 1

#include <jack/jack.h>
#include <graphline.h>
#include <atomickit/atomic-list.h>
#include <sonicmaths/jbridge.h>

/**
 * Midi from JACK
 */
struct smaths_jmidi {
    struct smaths_jbridge *jbridge;
    struct gln_socket ctl;
    struct gln_socket note;
    struct gln_socket vel;
    jack_port_t *midi_port;
    struct smaths_jbridge_untied_port stub;
    float note_val;
    float vel_val;
    unsigned char vel_prefix;
    unsigned char channel;
};

/**
 * Destroy JACK Midi
 *
 * @returns @c 0 on success, nonzero otherwise.
 */
int smaths_jmidi_destroy(struct smaths_jmidi *jmidi);

/**
 * Initialize JACK Midi
 *
 * @returns @c 0 on success, nonzero otherwise.
 */
int smaths_jmidi_init(struct smaths_jmidi *jmidi, struct smaths_jbridge *jbridge, unsigned char channel);

#endif
