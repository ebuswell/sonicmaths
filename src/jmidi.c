/*
 * jmidi.c
 * 
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
#include <alloca.h>
#include <stdio.h>
#include <jack/jack.h>
#include <jack/midiport.h>
#include <atomickit/atomic.h>
#include <atomickit/atomic-list.h>
#include <graphline.h>
#include "sonicmaths/graph.h"
#include "sonicmaths/jmidi.h"

static int smaths_jmidi_process_from_jack(jack_nframes_t nframes, struct smaths_jmidi *self) {
    void *midi_buffer = jack_port_get_buffer(self->midi_port, nframes);
    if(midi_buffer == NULL) {
	return -1;
    }
    float *note_buffer = gln_socket_get_buffer(&self->note);
    if(note_buffer == NULL) {
	return -1;
    }
    float *ctl_buffer = gln_socket_get_buffer(&self->ctl);
    if(ctl_buffer == NULL) {
	return -1;
    }
    float *vel_buffer = gln_socket_get_buffer(&self->vel);
    if(vel_buffer == NULL) {
	return -1;
    }

    jack_nframes_t N;
    N = jack_midi_get_event_count(midi_buffer);

    jack_nframes_t i;
    size_t j = 0;
    for(i = 0; i < N; i++) {
	jack_midi_event_t event;
	int r;

	r = jack_midi_event_get(&event, midi_buffer, i);
	if(r != 0) {
	    continue;
	}
	if(event.size == 0) {
	    continue;
	}
	if((event.buffer[0] & 0x0f) != self->channel) {
	    continue;
	}
	for(; j < event.time; j++) {
	    note_buffer[j] = self->note_val;
	    vel_buffer[j] = self->vel_val;
	    /* pitch_buffer[j] = self->pitch_val; */
	    ctl_buffer[j] = 0.0f;
	}
	float ctl = 0.0f;
	switch(event.buffer[0] & 0xf0) {
	case 0x80:
	    /* Note Off */
	    if(event.size < 3) {
		break;
	    }
	    self->note_val = (float) (((int) event.buffer[1]) - 60);
	    if(self->vel_prefix != 0) {
		/* self->vel_val = ((float) ((int) self->vel_prefix + ((int) event.buffer[2] << 7))) / 16383.0f; */
		self->vel_prefix = 0;
	    }/*  else { */
	    /* 	self->vel_val = ((float) event.buffer[2]) / 127.0f; */
	    /* } */
	    ctl = -1.0f;
	    break;
	case 0x90:
	    /* Note On */
	    if(event.size < 3) {
		break;
	    }
	    self->note_val = (float) (((int) event.buffer[1]) - 60);
	    if(self->vel_prefix != 0) {
		self->vel_val = ((float) ((int) self->vel_prefix + ((int) event.buffer[2] << 7))) / 16383.0f;
		self->vel_prefix = 0;
	    } else {
		self->vel_val = ((float) event.buffer[2]) / 127.0f;
	    }
	    ctl = 1.0f;
	    break;
	/* case 0xa0: */
	/*     /\* Polyphonic Aftertouch *\/ */
	/*     if(event.size < 3) { */
	/* 	break; */
	/*     } */
	/*     self->note_val = (float) (((int) event.buffer[1]) - 60); */
	/*     self->vel_val = ((float) event.buffer[2]) / 127.0f; */
	/*     break; */
	case 0xb0:
	    /* Control Change */
	    if(event.size < 3) {
		break;
	    }
	    switch(event.buffer[1]) {
	    /* case 0x00: */
	    /* 	/\* Bank Select MSB *\/ */
	    /* case 0x20: */
	    /* 	/\* Bank Select LSB *\/ */
	    /* case 0x01: */
	    /* 	/\* Modulation Wheel MSB *\/ */
	    /* case 0x21: */
	    /* 	/\* Modulation Wheel LSB *\/ */
	    /* case 0x02: */
	    /* 	/\* Breath Controller MSB *\/ */
	    /* case 0x22: */
	    /* 	/\* Breath Controller LSB *\/ */
	    /* case 0x04: */
	    /* 	/\* Foot Controller MSB *\/ */
	    /* case 0x24: */
	    /* 	/\* Foot Controller LSB *\/ */
	    /* case 0x05: */
	    /* 	/\* Portamento Time MSB *\/ */
	    /* case 0x25: */
	    /* 	/\* Portamento Time LSB *\/ */
	    /* case 0x07: */
	    /* 	/\* Channel Volume MSB *\/ */
	    /* case 0x27: */
	    /* 	/\* Channel Volume LSB *\/ */
	    /* case 0x08: */
	    /* 	/\* Balance MSB *\/ */
	    /* case 0x28: */
	    /* 	/\* Balance LSB *\/ */
	    /* case 0x0A: */
	    /* 	/\* Pan MSB *\/ */
	    /* case 0x2A: */
	    /* 	/\* Pan LSB *\/ */
	    /* case 0x0B: */
	    /* 	/\* Expression Controller MSB *\/ */
	    /* case 0x2B: */
	    /* 	/\* Expression Controller LSB *\/ */
	    /* case 0x0C: */
	    /* 	/\* Effect Control 1 MSB *\/ */
	    /* case 0x2C: */
	    /* 	/\* Effect Control 1 LSB *\/ */
	    /* case 0x0D: */
	    /* 	/\* Effect Control 2 MSB *\/ */
	    /* case 0x2D: */
	    /* 	/\* Effect Control 2 LSB *\/ */
	    /* case 0x10: */
	    /* 	/\* General Purpose Controller 1 MSB *\/ */
	    /* case 0x30: */
	    /* 	/\* General Purpose Controller 1 LSB *\/ */
	    /* case 0x11: */
	    /* 	/\* General Purpose Controller 2 MSB *\/ */
	    /* case 0x31: */
	    /* 	/\* General Purpose Controller 2 LSB *\/ */
	    /* case 0x12: */
	    /* 	/\* General Purpose Controller 3 MSB *\/ */
	    /* case 0x32: */
	    /* 	/\* General Purpose Controller 3 LSB *\/ */
	    /* case 0x13: */
	    /* 	/\* General Purpose Controller 4 MSB *\/ */
	    /* case 0x33: */
	    /* 	/\* General Purpose Controller 4 LSB *\/ */
	    /* case 0x46: */
	    /* 	/\* Sound Variation *\/ */
	    /* case 0x47: */
	    /* 	/\* Timbre / Harmonic Intensity *\/ */
	    /* case 0x48: */
	    /* 	/\* Release Time *\/ */
	    /* case 0x49: */
	    /* 	/\* Attack Time *\/ */
	    /* case 0x4A: */
	    /* 	/\* Brightness *\/ */
	    /* case 0x4B: */
	    /* 	/\* Decay Time *\/ */
	    /* case 0x4C: */
	    /* 	/\* Vibrato Rate *\/ */
	    /* case 0x4D: */
	    /* 	/\* Vibrato Depth *\/ */
	    /* case 0x4E: */
	    /* 	/\* Vibrato Delay *\/ */
	    /* case 0x4F: */
	    /* 	/\* Sound Controller 10 *\/ */
	    /* case 0x50: */
	    /* 	/\* General Purpose Controller 5 *\/ */
	    /* case 0x51: */
	    /* 	/\* General Purpose Controller 6 *\/ */
	    /* case 0x52: */
	    /* 	/\* General Purpose Controller 7 *\/ */
	    /* case 0x53: */
	    /* 	/\* General Purpose Controller 8 *\/ */
	    /* case 0x54: */
	    /* 	/\* Portamento Control *\/ */
	    /* case 0x5B: */
	    /* 	/\* Reverb Send Level *\/ */
	    /* case 0x5C: */
	    /* 	/\* Effects 2 Depth *\/ */
	    /* case 0x5D: */
	    /* 	/\* Effects 3 Depth *\/ */
	    /* case 0x5E: */
	    /* 	/\* Effects 4 Depth *\/ */
	    /* case 0x5F: */
	    /* 	/\* Effects 5 Depth *\/ */


	    /* case 0x40: */
	    /* 	/\* Damper Pedal on/off *\/ */
	    /* case 0x41: */
	    /* 	/\* Portamento on/off *\/ */
	    /* case 0x42: */
	    /* 	/\* Sostenuto on/off *\/ */
	    /* case 0x43: */
	    /* 	/\* Soft Pedal on/off *\/ */
	    /* case 0x44: */
	    /* 	/\* Legato Footswitch on/off *\/ */
	    /* case 0x45: */
	    /* 	/\* Hold 2 on/off *\/ */


	    case 0x58:
		/* High Resolution Velocity Prefix */
		self->vel_prefix = event.buffer[2];
		break;

	    /* case 0x62: */
	    /* 	/\* Non-Registered Parameter Number LSB *\/ */
	    /* case 0x63: */
	    /* 	/\* Non-Registered Parameter Number MSB *\/ */
	    /* case 0x65: */
	    /* 	/\* Registered Parameter Number MSB *\/ */
	    /* case 0x64: */
	    /* 	/\* Registered Parameter Number LSB *\/ */
	    /* case 0x06: */
	    /* 	/\* Data Entry MSB *\/ */
	    /* case 0x26: */
	    /* 	/\* Data Entry LSB *\/ */
	    /* case 0x60: */
	    /* 	/\* Data Increment *\/ */
	    /* case 0x61: */
	    /* 	/\* Data Decrement *\/ */
	    /* 	switch(registered_number) { */
	    /* 	case 0x0000: */
	    /* 	    /\* Pitch Bend Sensitivity *\/ */
	    /* 	case 0x0001: */
	    /* 	    /\* Channel Fine Tuning *\/ */
	    /* 	case 0x0002: */
	    /* 	    /\* Channel Coarse Tuning *\/ */
	    /* 	case 0x0003: */
	    /* 	    /\* Tuning Program Change *\/ */
	    /* 	case 0x0004: */
	    /* 	    /\* Tuning Bank Select*\/ */
	    /* 	case 0x0005: */
	    /* 	    /\* Modulation Depth Range *\/ */
	    /* 	case 0x3D00: */
	    /* 	    /\* Azimuth Angle *\/ */
	    /* 	case 0x3D01: */
	    /* 	    /\* Elevation Angle *\/ */
	    /* 	case 0x3D02: */
	    /* 	    /\* Gain *\/ */
	    /* 	case 0x3D03: */
	    /* 	    /\* Distance Ratio *\/ */
	    /* 	case 0x3D04: */
	    /* 	    /\* Maximum Distance *\/ */
	    /* 	case 0x3D05: */
	    /* 	    /\* Gain at Maximum Distance *\/ */
	    /* 	case 0x3D06: */
	    /* 	    /\* Reference Distance Ratio *\/ */
	    /* 	case 0x3D07: */
	    /* 	    /\* Pan Spread Angle *\/ */
	    /* 	case 0x3D08: */
	    /* 	    /\* Roll Angle *\/ */
	    /* 	} */

	    case 0x78:
		/* All Sound Off */
		self->vel_val = 0.0f;
		break;
	    case 0x79:
		/* Reset All Controllers */
		self->note_val = 0.0f;
		self->vel_val = 1.0f;
		/* self->pitch_val = 0.0f; */
		ctl = 0.0;
		break;
	    /* case 0x7A: */
	    /* 	/\* Local Control *\/ */
	    /* 	break; */
	    case 0x7B:
		/* All Notes Off */
	    case 0x7C:
		/* Omni Mode Off */
	    case 0x7D:
		/* Omni Mode On */
	    case 0x7E:
		/* Mono Mode On */
	    case 0x7F:
		/* Poly Mode On */
		/* All of these effectively turn off all notes */
		/* ctl = -1; */
		break;
	    }
	    break;
	/* case 0xc0: */
	/*     /\* Program Change *\/ */
	/*     break; */
	/* case 0xd0: */
	/*     /\* Channel Aftertouch *\/ */
	/*     if(event.size < 2) { */
	/* 	break; */
	/*     } */
	/*     self->vel_val = ((float) event.buffer[1]) / 127.0f; */
	/*     break; */
	/* case 0xe0: */
	/*     /\* Pitch Wheel *\/ */
	/*     if(event.size < 3) { */
	/*     	break; */
	/*     } */
	/*     self->pitch_val = ((float) (((int) event.buffer[1] + (int) event.buffer[2] << 7) - 8192)) / 8192.0f; */
	/*     break; */
	/* default: */
	/*     /\* System Message *\/ */
	/*     break; */
	}
	note_buffer[j] = self->note_val;
	vel_buffer[j] = self->vel_val;
	/* pitch_buffer[j] = self->pitch_val; */
	ctl_buffer[j++] = ctl;
    }
    for(; j < self->jbridge->graph.graph.buffer_nmemb; j++) {
	note_buffer[j] = self->note_val;
	vel_buffer[j] = self->vel_val;
	/* pitch_buffer[j] = self->pitch_val; */
	ctl_buffer[j] = 0.0f;
    }
    return 0;
}

int smaths_jmidi_init(struct smaths_jmidi *self, struct smaths_jbridge *jbridge, unsigned char channel) {
    self->note_val = 0.0f;
    self->vel_val = 1.0f;
    self->vel_prefix = 0;
    self->channel = channel;
    self->jbridge = jbridge;
    int r;

    r = gln_socket_init(&self->ctl, &jbridge->graph.node, OUTPUT);
    if(r != 0) {
	return r;
    }

    r = gln_socket_init(&self->note, &jbridge->graph.node, OUTPUT);
    if(r != 0) {
	gln_socket_destroy(&self->ctl);
	return r;
    }

    r = gln_socket_init(&self->vel, &jbridge->graph.node, OUTPUT);
    if(r != 0) {
	gln_socket_destroy(&self->note);
	gln_socket_destroy(&self->ctl);
	return r;
    }

    char *tmpnam = alloca(32);
    r = snprintf(tmpnam, 32, "%d-MIDI_%d", atomic_inc_return(&jbridge->portnum), channel);
    if(r < 0) {
	gln_socket_destroy(&self->vel);
	gln_socket_destroy(&self->note);
	gln_socket_destroy(&self->ctl);
	return r;
    }

    self->midi_port = jack_port_register(jbridge->client, tmpnam, JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    if(self->midi_port == NULL) {
	gln_socket_destroy(&self->vel);
	gln_socket_destroy(&self->note);
	gln_socket_destroy(&self->ctl);
	return -1;
    }

    self->stub.cb = (JackProcessCallback) smaths_jmidi_process_from_jack;
    self->stub.arg = self;

    r = atomic_list_push(&jbridge->untied_ports, &self->stub);
    if(r != 0) {
	jack_port_unregister(self->jbridge->client, self->midi_port);
	gln_socket_destroy(&self->vel);
	gln_socket_destroy(&self->note);
	gln_socket_destroy(&self->ctl);
	return r;
    }
    return 0;
}

int smaths_jmidi_destroy(struct smaths_jmidi *self) {
    gln_socket_disconnect(&self->vel);
    gln_socket_disconnect(&self->note);
    gln_socket_disconnect(&self->ctl);
    atomic_list_remove_by_value(&self->jbridge->untied_ports, &self->stub);
    int r = jack_port_unregister(self->jbridge->client, self->midi_port);
    gln_socket_destroy(&self->vel);
    gln_socket_destroy(&self->note);
    gln_socket_destroy(&self->ctl);
    return r;
}
