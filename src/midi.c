/*
 * midi.c
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

#include <stdint.h>
#include <stdbool.h>
#include <atomickit/atomic.h>
#include <atomickit/atomic-rcp.h>
#include <atomickit/atomic-malloc.h>
#include <graphline.h>
#include "sonicmaths/buffer.h"
#include "sonicmaths/graph.h"
#include "sonicmaths/controller.h"
#include "sonicmaths/midi.h"

#include <stdio.h>

static float smaths_midi_note_to_note(int8_t key_root, bool chromatic, uint8_t note) {
    int8_t ret = ((int8_t) note) - key_root;
    if(!chromatic) {
	/* convert to proper scale */
	int8_t octaves = ret / 12;
	int8_t rem = ret % 12;
	if(rem < 0) {
	    rem += 12;
	    octaves--;
	}
	switch(rem) {
	case 0:
	    return (float) (octaves * 7);
	case 1:
	    return (float) (octaves * 7) + 0.5f;
	case 2:
	    return (float) (octaves * 7 + 1);
	case 3:
	    return (float) (octaves * 7 + 1) + 0.5f;
	case 4:
	    return (float) (octaves * 7 + 2);
	case 5:
	    return (float) (octaves * 7 + 3);
	case 6:
	    return (float) (octaves * 7 + 3) + 0.5f;
	case 7:
	    return (float) (octaves * 7 + 4);
	case 8:
	    return (float) (octaves * 7 + 4) + 0.5f;
	case 9:
	    return (float) (octaves * 7 + 5);
	case 10:
	    return (float) (octaves * 7 + 5) + 0.5f;
	case 11:
	    return (float) (octaves * 7 + 6);
	}
    }
    return (float) ret;
}

static void smaths_midi_note_off(struct smaths_midi_ctlr *ctlr, struct smaths_buffer *ctl_buffer, int frame, bool mono_on, float note, float velocity) {
    int nvoices = ctlr->nvoices;
    int voice;
    int i;
    printf("Note off: frame: %d, mono_on: %s, note: %f, velocity: %f\n", frame, mono_on ? "true" : "false", (double) note, (double) velocity);
    if(mono_on) {
	if(ctlr->voices[0].note != note) {
	    return;
	}
	voice = 0;
    } else {
	/* Find the note */
	voice = -1;
	for(i = 0; i < nvoices; i++) {
	    if(ctlr->voices[i].note == note) {
		voice = i;
		break;
	    }
	}
	if(voice == -1) {
	    return;
	}
	printf("Note assigned to voice %d\n", voice);
 	/* Find the voice */
	for(i = 0; i < nvoices; i++) {
	    if(ctlr->voices_lru_start[i] == voice) {
		printf("Voice in start queue at %d\n", i);
		memmove(ctlr->voices_lru_start + i, ctlr->voices_lru_start + i + 1, sizeof(int) * (nvoices - (i + 1)));
		ctlr->voices_lru_start[nvoices - 1] = -1;
		for(i = 0; ctlr->voices_lru_stop[i] != -1; i++);
		ctlr->voices_lru_stop[i] = voice;
		break;
	    }
	}
	if(i == nvoices) {
	    return;
	}
    }
    printf("End note off\n");
    for(i = 0; i < nvoices; i++) {
	printf("LRU_start[%d] = %d\n", i, ctlr->voices_lru_start[i]);
    }
    for(i = 0; i < nvoices; i++) {
	printf("LRU_stop[%d] = %d\n", i, ctlr->voices_lru_stop[i]);
    }
    ctlr->voices[voice].release_velocity = velocity;
    ctl_buffer->data[frame*nvoices + voice] = -1;
}


static inline void smaths_midi_note_on(struct smaths_midi_ctlr *ctlr, struct smaths_buffer *ctl_buffer, int frame, bool mono_on, float note, float velocity) {
    int nvoices = ctlr->nvoices;
    int voice;
    int i;
    printf("Note on: frame: %d, mono_on: %s, note: %f, velocity: %f\n", frame, mono_on ? "true" : "false", (double) note, (double) velocity);
    if(velocity == 0) {
	/* Note off */
	smaths_midi_note_off(ctlr, ctl_buffer, frame, mono_on, note, 0.5f);
	return;
    }
    if(mono_on) {
	voice = 0;
    } else {
	/* Find the note */
	voice = -1;
	for(i = 0; i < nvoices; i++) {
	    if(ctlr->voices[i].note == note) {
		voice = i;
		break;
	    }
	}
	if(voice != -1) {
	    printf("Note already assigned to voice %d\n", voice);
	    for(i = 0; i < nvoices; i++) {
		if(ctlr->voices_lru_start[i] == voice) {
		    printf("Voice already in start queue at %d\n", i);
		    /* Found it! */
		    memmove(ctlr->voices_lru_start + i, ctlr->voices_lru_start + i + 1, sizeof(int) * (nvoices - (i + 1)));
		    ctlr->voices_lru_start[nvoices - 1] = -1;
		    for(i = 0; ctlr->voices_lru_start[i] != -1; i++);
		    ctlr->voices_lru_start[i] = voice;
		    break;
		}
	    }
	    if(i == nvoices) {
		printf("Voice was stopped\n");
		/* The voice must be currently stopped */
		for(i = 0; ctlr->voices_lru_stop[i] != voice; i++);
		printf("Voice found in stop queue at %d\n", i);
		memmove(ctlr->voices_lru_stop + i, ctlr->voices_lru_stop + i + 1, sizeof(int) * (nvoices - (i + 1)));
		ctlr->voices_lru_stop[nvoices - 1] = -1;
		for(i = 0; ctlr->voices_lru_start[i] != -1; i++);
		ctlr->voices_lru_start[i] = voice;
	    }
	} else {
	    printf("Note not yet assigned to voice\n");
	    /* Allocate a new voice to this note */
	    if(ctlr->voices_lru_stop[0] != -1) {
		printf("Assigning LRU stopped voice %d\n", ctlr->voices_lru_stop[0]);
		/* least recently used stopped voice */
		voice = ctlr->voices_lru_stop[0];
		memmove(ctlr->voices_lru_stop, ctlr->voices_lru_stop + 1, sizeof(int) * (nvoices - 1));
		ctlr->voices_lru_stop[nvoices - 1] = -1;
		for(i = 0; ctlr->voices_lru_start[i] != -1; i++);
		ctlr->voices_lru_start[i] = voice;
	    } else {
		printf("Assigning LRU started voice %d\n", ctlr->voices_lru_start[0]);
		/* least recently used started voice */
		voice = ctlr->voices_lru_start[0];
		memmove(ctlr->voices_lru_start, ctlr->voices_lru_start + 1, sizeof(int) * (nvoices - 1));
		ctlr->voices_lru_start[nvoices - 1] = -1;
		for(i = 0; ctlr->voices_lru_start[i] != -1; i++);
		ctlr->voices_lru_start[i] = voice;
	    }
	}
    }
    printf("End note on\n");
    for(i = 0; i < nvoices; i++) {
	printf("LRU_start[%d] = %d\n", i, ctlr->voices_lru_start[i]);
    }
    for(i = 0; i < nvoices; i++) {
	printf("LRU_stop[%d] = %d\n", i, ctlr->voices_lru_stop[i]);
    }
    ctlr->voices[voice].note = note;
    ctlr->voices[voice].attack_velocity = velocity;
    ctl_buffer->data[frame*nvoices + voice] = 1;
}

static void smaths_midi_all_notes_off(struct smaths_midi_ctlr *ctlr, struct smaths_buffer *ctl_buffer, int frame) {
    int i;
    for(i = 0; i < ctlr->nvoices; i++) {
	if(ctlr->voices_lru_start[i] == -1) {
	    break;
	}
	ctl_buffer->data[frame * ctlr->nvoices + ctlr->voices_lru_start[i]] = -1;
	ctlr->voices_lru_stop[(ctlr->nvoices - 1) - i] = ctlr->voices_lru_start[i];
	ctlr->voices_lru_start[i] = -1;
    }
}

static inline void smaths_midi_pressure_change(struct smaths_midi_ctlr *ctlr, float note, float pressure) {
    int i;
    for(i = 0; i < ctlr->nvoices; i++) {
	if(ctlr->voices[i].note == note) {
	    ctlr->voices[i].pressure = pressure;
	    break;
	}
    }
}

static inline int smaths_midi_ctlr_redim_state(struct smaths_midi_ctlr *ctlr, int nvoices) {
    if(nvoices != ctlr->nvoices) {
	int i, j;
	struct smaths_midi_voice *voices;
	int *voices_lru_start;
	voices = arealloc(ctlr->voices,
			  ctlr->nvoices * sizeof(struct smaths_midi_voice),
			  nvoices * sizeof(struct smaths_midi_voice));
	if(voices == NULL) {
	    return -1;
	}
	if(ctlr->nvoices < nvoices) {
	    memset(voices + ctlr->nvoices, 0, (nvoices - ctlr->nvoices) * sizeof(struct smaths_midi_voice));
	}
	ctlr->voices = voices;
	voices_lru_start = arealloc(ctlr->voices_lru_start, ctlr->nvoices * sizeof(int), nvoices * sizeof(int));
	if(voices_lru_start == NULL) {
	    afree(ctlr->voices, nvoices * sizeof(struct smaths_midi_voice));
	    afree(ctlr->voices_lru_start, ctlr->nvoices * sizeof(int));
	    afree(ctlr->voices_lru_stop, ctlr->nvoices * sizeof(int));
	    ctlr->nvoices = 0;
	    return -1;
	}
	for(i = ctlr->nvoices; i < nvoices; i++) {
	    voices_lru_start[i] = -1;
	}
	ctlr->voices_lru_start = voices_lru_start;
	if(atryrealloc(ctlr->voices_lru_stop, ctlr->nvoices * sizeof(int), nvoices * sizeof(int))) {
	    if(nvoices > ctlr->nvoices) {
		/* All new voices jump the queue */
		memmove(&ctlr->voices_lru_stop[nvoices - ctlr->nvoices], ctlr->voices_lru_stop, ctlr->nvoices * sizeof(int));
	    }
	} else {
	    int *voices_lru_stop = amalloc(nvoices * sizeof(int));
	    if(voices_lru_stop == NULL) {
		afree(ctlr->voices, nvoices * sizeof(struct smaths_midi_voice));
		afree(ctlr->voices_lru_start, nvoices * sizeof(int));
		afree(ctlr->voices_lru_stop, ctlr->nvoices * sizeof(int));
		ctlr->nvoices = 0;
		return -1;
	    }
	    if(nvoices > ctlr->nvoices) {
		/* All new voices jump the queue */
		memcpy(&voices_lru_stop[nvoices - ctlr->nvoices], ctlr->voices_lru_stop, ctlr->nvoices * sizeof(int));
	    } else {
		memcpy(voices_lru_stop, ctlr->voices_lru_stop, nvoices * sizeof(int));
	    }
	    afree(ctlr->voices_lru_stop, ctlr->nvoices * sizeof(int));
	    ctlr->voices_lru_stop = voices_lru_stop;
	}
	for(j = 0, i = ctlr->nvoices; i < nvoices; i++) {
	    ctlr->voices_lru_stop[j++] = i;
	}
	ctlr->nvoices = nvoices;
    }
    return 0;
}

static int smaths_midi_ctlr_process(struct smaths_midi_ctlr *ctlr) {
    int i, j, r;

    struct smaths_midi_buffer *midi_buffer;
    struct smaths_buffer *out_buffer;
    struct smaths_buffer *ctl_buffer;
    struct smaths_buffer *attack_velocity_buffer;
    struct smaths_buffer *release_velocity_buffer;
    struct smaths_buffer *pressure_buffer;

    int nframes, nchannels, nvoices;

    uint8_t midi_channel;
    int8_t key_root;
    bool chromatic;
    bool omni_on, mono_on;

    float attack_velocity_min, attack_velocity_max, release_velocity_min, release_velocity_max, pressure_min, pressure_max;

    r = gln_get_buffers(1, ctlr->midi_in, &midi_buffer);
    if(r != 0) {
	return r;
    }

    nframes = smaths_node_frames_per_period(ctlr);
    if(nframes < 0) {
	return nframes;
    }

    nchannels = midi_buffer == NULL ? 1 : midi_buffer->nchannels;

    nvoices = atomic_load_explicit(&ctlr->user_nvoices, memory_order_consume);

    r = smaths_midi_ctlr_redim_state(ctlr, nvoices);
    if(r != 0) {
	return r;
    }

    out_buffer = smaths_alloc_buffer(ctlr->out, nframes, nvoices);
    if(out_buffer == NULL) {
	return -1;
    }
    ctl_buffer = smaths_alloc_buffer(ctlr->ctl, nframes, nvoices);
    if(ctl_buffer == NULL) {
	return -1;
    }
    attack_velocity_buffer = smaths_alloc_buffer(ctlr->attack_velocity, nframes, nvoices);
    if(attack_velocity_buffer == NULL) {
	return -1;
    }
    release_velocity_buffer = smaths_alloc_buffer(ctlr->release_velocity, nframes, nvoices);
    if(release_velocity_buffer == NULL) {
	return -1;
    }
    pressure_buffer = smaths_alloc_buffer(ctlr->pressure, nframes, nvoices);
    if(pressure_buffer == NULL) {
	return -1;
    }

    omni_on = atomic_load_explicit(&ctlr->omni_on, memory_order_consume);
    mono_on = atomic_load_explicit(&ctlr->mono_on, memory_order_consume);

    key_root = atomic_load_explicit(&ctlr->key_root, memory_order_consume);
    chromatic = atomic_load_explicit(&ctlr->chromatic, memory_order_consume);

    midi_channel = atomic_load_explicit(&ctlr->midi_channel, memory_order_consume);

    attack_velocity_min = atomic_float_load_explicit(&ctlr->attack_velocity_min, memory_order_consume);
    attack_velocity_max = atomic_float_load_explicit(&ctlr->attack_velocity_max, memory_order_consume);
    release_velocity_min = atomic_float_load_explicit(&ctlr->release_velocity_min, memory_order_consume);
    release_velocity_max = atomic_float_load_explicit(&ctlr->release_velocity_max, memory_order_consume);
    pressure_min = atomic_float_load_explicit(&ctlr->pressure_min, memory_order_consume);
    pressure_max = atomic_float_load_explicit(&ctlr->pressure_max, memory_order_consume);

    memset(ctl_buffer->data, 0, 4 * nframes * nvoices);

    if(mono_on != ctlr->last_mono_on) {
	/* Turn all notes off. */
	smaths_midi_all_notes_off(ctlr, ctl_buffer, 0);
	ctlr->last_mono_on = mono_on;
    }

    for(i = 0; i < nframes; i++) {
	if(midi_buffer != NULL) {
	    for(j = 0; j < nchannels; j++) {
		/* Process midi message */
		uint8_t *message = &midi_buffer->data[(i * nchannels + j) * 4];
		if((message[0] & 0x80) == 0) {
		    /* No or malformed message */
		    goto next_message;
		}
		if(!omni_on) {
		    if(((message[0] & 0xF0) != 0xF0)
		       && ((message[0] & 0xF) != midi_channel)) {
			/* Channel message not for our channel */
			goto next_message;
		    }
		}
		/* Process voice data */
		switch(message[0]) {
		case 0x80:
		    /* Note Off */
		    if(((message[1] & 0x80) != 0)
		       || ((message[2] & 0x80) != 0)) {
			/* Malformed or partial message */
			goto next_message;
		    }
		    smaths_midi_note_off(ctlr, ctl_buffer, i, mono_on,
					 smaths_midi_note_to_note(key_root, chromatic, message[1]),
					 ctlr->HRVP >= 0 ? (((float) ((((int) message[2]) << 7) + ctlr->HRVP))
							    / ((float) (1 << 14)))
					 : (((float) message[2])
					    / ((float) (1 << 7))));
		    ctlr->HRVP = -1;
		    break;
		case 0x90:
		    /* Note On */
		    if(((message[1] & 0x80) != 0)
		       || ((message[2] & 0x80) != 0)) {
			/* Malformed or partial message */
			goto next_message;
		    }
		    smaths_midi_note_on(ctlr, ctl_buffer, i, mono_on,
					smaths_midi_note_to_note(key_root, chromatic, message[1]),
					ctlr->HRVP >= 0 ? (((float) ((((int) message[2]) << 7) + ctlr->HRVP))
							   / ((float) (1 << 14)))
					: (((float) message[2])
					   / ((float) (1 << 7))));
		    ctlr->HRVP = -1;
		    break;
		case 0xA0:
		    /* Polyphonic Pressure */
		    smaths_midi_pressure_change(ctlr,
						smaths_midi_note_to_note(key_root, chromatic, message[1]),
						((float) message[2])
						/ ((float) (1 << 7)));
		    break;
		case 0xB0:
		    if((message[1] & 0x80) != 0) {
			/* Malformed or partial message */
			goto next_message;
		    }
		    /* Control Change */
		    switch(message[1]) {
		    case 0x58:
			/* High Resolution Velocity Prefix */
			if((message[2] & 0x80) != 0) {
			    /* Malformed or partial message */
			    goto next_message;
			}
			ctlr->HRVP = message[2];
			break;
		    case 0x78:
			/* All Sound Off */
			memset(ctlr->voices, 0, sizeof(struct smaths_midi_voice) * nvoices);
			smaths_midi_all_notes_off(ctlr, ctl_buffer, i);
			break;
		    /* case 0x79: */
		    /* 	/\* Reset All Controllers *\/ */
		    /* case 0x7A: */
		    /* 	/\* Local Control on/off *\/ */
		    case 0x7B:
			/* All Notes off */
			smaths_midi_all_notes_off(ctlr, ctl_buffer, i);
			break;
		    case 0x7C:
			/* Omni Mode Off & All notes off */
			omni_on = false;
			atomic_store_explicit(&ctlr->omni_on, false, memory_order_release);
			smaths_midi_all_notes_off(ctlr, ctl_buffer, i);
			break;
		    case 0x7D:
			/* Omni Mode On & All notes off */
			omni_on = true;
			atomic_store_explicit(&ctlr->omni_on, true, memory_order_release);
			smaths_midi_all_notes_off(ctlr, ctl_buffer, i);
			break;
		    case 0x7E:
			/* Mono Mode On & All notes off */
			mono_on = true;
			atomic_store_explicit(&ctlr->mono_on, true, memory_order_release);
			ctlr->last_mono_on = true;
			smaths_midi_all_notes_off(ctlr, ctl_buffer, i);
			break;
		    case 0x7F:
			/* Poly Mode On & All notes off */
			mono_on = false;
			atomic_store_explicit(&ctlr->mono_on, false, memory_order_release);
			ctlr->last_mono_on = false;
			smaths_midi_all_notes_off(ctlr, ctl_buffer, i);
			break;
		    }
		    break;
		/* case 0xF0: */
		/*     if(ret->data[0] & 0x8) { */
		/* 	/\* Real time message *\/ */
		/* 	switch(ret->data[0] & 0x7) { */
		/* 	case 0x0: */
		/* 	    /\* Timing clock *\/ */
		/* 	case 0x1: */
		/* 	    /\* Undefined *\/ */
		/* 	case 0x2: */
		/* 	    /\* Start *\/ */
		/* 	case 0x3: */
		/* 	    /\* Continue *\/ */
		/* 	case 0x4: */
		/* 	    /\* Stop *\/ */
		/* 	case 0x5: */
		/* 	    /\* Undefined *\/ */
		/* 	case 0x6: */
		/* 	    /\* Active Sensing *\/ */
		/* 	case 0x7: */
		/* 	    /\* System Reset *\/ */
		/* 	} */
		/*     } else { */
		/* 	/\* System Messages *\/ */
		/* 	switch(ret->data[0] & 0x7) { */
		/* 	case 0x0: */
		/* 	    /\* System Exclusive *\/ */
		/* 	case 0x1: */
		/* 	    /\* Midi Time Code Quarter Frame *\/ */
		/* 	case 0x2: */
		/* 	    /\* Song Position Pointer *\/ */
		/* 	case 0x3: */
		/* 	    /\* Song Select *\/ */
		/* 	case 0x4: */
		/* 	    /\* Undefined *\/ */
		/* 	case 0x5: */
		/* 	    /\* Undefined *\/ */
		/* 	case 0x6: */
		/* 	    /\* Tune Request *\/ */
		/* 	case 0x7: */
		/* 	    /\* End System Exclusive *\/ */
		/* 	} */
		/*     } */
		}
	    next_message:
		continue;
	    }
	}
	/* Emit stored data */
	for(j = 0; j < nvoices; j++) {
	    attack_velocity_buffer->data[i*nvoices + j] = ctlr->voices[j].attack_velocity * (attack_velocity_max - attack_velocity_min) + attack_velocity_min;
	    release_velocity_buffer->data[i*nvoices + j] = ctlr->voices[j].release_velocity * (release_velocity_max - release_velocity_min) + release_velocity_min;
	    pressure_buffer->data[i*nvoices + j] = ctlr->voices[j].pressure * (pressure_max - pressure_min) + pressure_min;
	    out_buffer->data[i*nvoices + j] = ctlr->voices[j].note;
	}
    }
    return 0;
}

int smaths_midi_ctlr_init(struct smaths_midi_ctlr *ctlr, struct smaths_graph *graph, void (*destroy)(struct smaths_midi_ctlr *ctlr)) {
    int r = -1;

    ctlr->voices = amalloc(sizeof(struct smaths_midi_voice) * 8);
    if(ctlr->voices == NULL) {
	goto undo0;
    }

    ctlr->voices_lru_start = amalloc(sizeof(int) * 8);
    if(ctlr->voices_lru_start == NULL) {
	goto undo1;
    }

    ctlr->voices_lru_stop = amalloc(sizeof(int) * 8);
    if(ctlr->voices_lru_stop == NULL) {
	goto undo2;
    }

    r = smaths_ctlr_init(ctlr, graph, (gln_process_fp_t) smaths_midi_ctlr_process, (void (*)(struct smaths_ctlr *)) destroy);
    if(r != 0) {
	goto undo3;
    }

    ctlr->midi_in = gln_socket_create(ctlr, GLNS_INPUT);
    if(ctlr->midi_in == NULL) {
	r = -1;
	goto undo4;
    }

    ctlr->attack_velocity = gln_socket_create(ctlr, GLNS_OUTPUT);
    if(ctlr->attack_velocity == NULL) {
	r = -1;
	goto undo5;
    }

    ctlr->release_velocity = gln_socket_create(ctlr, GLNS_OUTPUT);
    if(ctlr->release_velocity == NULL) {
	r = -1;
	goto undo6;
    }

    ctlr->pressure = gln_socket_create(ctlr, GLNS_OUTPUT);
    if(ctlr->pressure == NULL) {
	r = -1;
	goto undo7;
    }

    atomic_init(&ctlr->attack_velocity_min, 0);
    atomic_init(&ctlr->attack_velocity_max, 1);
    atomic_init(&ctlr->release_velocity_min, 0);
    atomic_init(&ctlr->release_velocity_max, 1);
    atomic_init(&ctlr->pressure_min, 0);
    atomic_init(&ctlr->pressure_max, 1);

    atomic_init(&ctlr->key_root, 60);
    atomic_init(&ctlr->chromatic, true);
    atomic_init(&ctlr->midi_channel, 0);
    atomic_init(&ctlr->omni_on, false);
    atomic_init(&ctlr->mono_on, false);
    ctlr->last_mono_on = false;
    atomic_init(&ctlr->user_nvoices, 8);
    ctlr->current_parameter = 0;
    ctlr->HRVP = -1;
    ctlr->nvoices = 8;
    memset(ctlr->voices, 0, sizeof(struct smaths_midi_voice) * 8);
    int i;
    for(i = 0; i < 8; i++) {
	ctlr->voices_lru_start[i] = -1;
	ctlr->voices_lru_stop[i] = i;
    }

    return 0;

undo7:
    arcp_release(ctlr->release_velocity);
undo6:
    arcp_release(ctlr->attack_velocity);
undo5:
    arcp_release(ctlr->midi_in);
undo4:
    smaths_ctlr_destroy(ctlr);
undo3:
    afree(ctlr->voices_lru_stop, sizeof(int));
undo2:
    afree(ctlr->voices_lru_start, sizeof(int));
undo1:
    afree(ctlr->voices, sizeof(struct smaths_midi_voice));
undo0:
    return r;
}

void smaths_midi_ctlr_destroy(struct smaths_midi_ctlr *ctlr) {
    arcp_release(ctlr->pressure);
    arcp_release(ctlr->release_velocity);
    arcp_release(ctlr->attack_velocity);
    arcp_release(ctlr->midi_in);
    smaths_ctlr_destroy(ctlr);
    afree(ctlr->voices_lru_stop, sizeof(int) * ctlr->nvoices);
    afree(ctlr->voices_lru_start, sizeof(int) * ctlr->nvoices);
    afree(ctlr->voices, sizeof(struct smaths_midi_voice) * ctlr->nvoices);
}

static void __smaths_midi_ctlr_destroy(struct smaths_midi_ctlr *ctlr) {
    smaths_midi_ctlr_destroy(ctlr);
    afree(ctlr, sizeof(struct smaths_midi_ctlr));
}

struct smaths_midi_ctlr *smaths_midi_ctlr_create(struct smaths_graph *graph) {
    struct smaths_midi_ctlr *ret;
    int r;

    ret = amalloc(sizeof(struct smaths_midi_ctlr));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_midi_ctlr_init(ret, graph, __smaths_midi_ctlr_destroy);
    if(r != 0) {
	afree(ret, sizeof(struct smaths_midi_ctlr));
	return NULL;
    }

    return ret;
}
