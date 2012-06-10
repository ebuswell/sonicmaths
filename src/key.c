/*
 * key.c
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

#include <stddef.h>
#include <string.h>
#include <atomickit/atomic-float.h>
#include <atomickit/spinlock.h>
#include <graphline.h>
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/key.h"

static const struct {
    size_t tuning_length;
    float tuning[7];
} _smaths_major_tuning = {
    7, {
	1.0,
	9.0/8.0,
	5.0/4.0,
	4.0/3.0,
	3.0/2.0,
	(4.0/3.0)*(5.0/4.0),
	(3.0/2.0)*(5.0/4.0)
    }
};

const struct smaths_key_tuning *SMATHS_MAJOR_TUNING = (struct smaths_key_tuning *) &_smaths_major_tuning;

static const struct {
    size_t tuning_length;
    float tuning[7];
} _smaths_minor_tuning = {
    7, {
	1.0,
	9.0/8.0,
	6.0/5.0,
	4.0/3.0,
	3.0/2.0,
	(4.0/3.0)*(6.0/5.0),
	(3.0/2.0)*(6.0/5.0)
    }
};

const struct smaths_key_tuning *SMATHS_MINOR_TUNING = (struct smaths_key_tuning *) &_smaths_minor_tuning;

const struct {
    size_t tuning_length;
    float tuning[12];
} _smaths_pythagorean_tuning = {
    12, {
	1.0,
	256.0/243.0,
	9.0/8.0,
	32.0/27.0,
	81.0/64.0,
	4.0/3.0,
	1024.0/729.0,
	32.0/27.0,
	128.0/81.0,
	27.0/16.0,
	16.0/9.0,
	243.0/128.0
    }
};

const struct smaths_key_tuning *SMATHS_PYTHAGOREAN_TUNING = (struct smaths_key_tuning *) &_smaths_pythagorean_tuning;

static inline float smaths_key_note2freq_param(float note, float root, const struct smaths_key_tuning *tuning) {
    if(tuning == SMATHS_EQUAL_TUNING) {
	return root * powf(2.0f, note/12.0f);
    } else {
	int tuning_length = tuning->tuning_length;
	float f = note;
	float n_f = floorf(f);
	f -= n_f;
	int n = (int) n_f;
	int m = n % tuning_length;
	int e = n / tuning_length;
	if(m < 0) {
	    e--;
	    m = tuning_length + m;
	}
	float freq = tuning->tuning[m];
	if(f != 0.0f) {
	    if(m == (tuning_length - 1)) {
		freq *= powf(2.0f/tuning->tuning[m], f);
	    } else {
		freq *= powf(tuning->tuning[m + 1]/tuning->tuning[m], f);
	    }
	}
	if(e >= 0) {
	    return (freq * root * ((float) (1 << e)));
	} else {
	    e = -e;
	    return (freq * root / ((float) (1 << e)));
	}
    }
}

static int smaths_key_process(struct smaths_key *self) {
    float *freq_buffer = gln_socket_get_buffer(&self->freq);
    if(freq_buffer == NULL) {
	return -1;
    }
    float *note_buffer = smaths_parameter_get_buffer(&self->note);
    if(note_buffer == NULL) {
	return -1;
    }
    float *root_buffer = smaths_parameter_get_buffer(&self->root);
    if(root_buffer == NULL) {
	return -1;
    }
    size_t i;
    spinlock_multilock(&self->tuning_lock);
    for(i = 0; i < self->graph->graph.buffer_nmemb; i++) {
	freq_buffer[i] = smaths_key_note2freq_param(note_buffer[i], root_buffer[i], self->tuning);
    }
    spinlock_unlock(&self->tuning_lock);
    atomic_float_set(&self->last_root, root_buffer[i - 1]);
    return 0;
}

float smaths_key_note2freq(struct smaths_key *self, float note) {
    float root = atomic_float_read(&self->root.value);
    if(isnanf(root)) {
	root = atomic_float_read(&self->last_root);
    }
    spinlock_multilock(&self->tuning_lock);
    float ret = smaths_key_note2freq_param(note, root, self->tuning);
    spinlock_unlock(&self->tuning_lock);
    return ret;
}

int smaths_key_set_tuning(struct smaths_key *self, const struct smaths_key_tuning *tuning) {
    if((tuning != SMATHS_EQUAL_TUNING)
       && (tuning != SMATHS_MAJOR_TUNING)
       && (tuning != SMATHS_MINOR_TUNING)
       && (tuning != SMATHS_PYTHAGOREAN_TUNING)) {
	const struct smaths_key_tuning *newtuning = malloc(sizeof(struct smaths_key_tuning) + (tuning->tuning_length - 1) * sizeof(float));
	if(newtuning == NULL) {
	    return -1;
	}
	memcpy(&newtuning, &tuning, sizeof(struct smaths_key_tuning) + (tuning->tuning_length - 1) * sizeof(float));
	tuning = newtuning;
    }
    const struct smaths_key_tuning *oldtuning;
    spinlock_lock(&self->tuning_lock);
    oldtuning = self->tuning;
    self->tuning = tuning;
    spinlock_unlock(&self->tuning_lock);
    if((oldtuning != SMATHS_MAJOR_TUNING)
       && (oldtuning != SMATHS_EQUAL_TUNING)
       && (oldtuning != SMATHS_MINOR_TUNING)
       && (oldtuning != SMATHS_PYTHAGOREAN_TUNING)) {
	free((void *) oldtuning);
    }
    return 0;
}

int smaths_key_init(struct smaths_key *self, struct smaths_graph *graph) {
    int r;
    self->graph = graph;

    r = gln_node_init(&self->node, &graph->graph, (gln_process_fp_t) smaths_key_process, self);
    if(r != 0) {
	return r;
    }

    r = gln_socket_init(&self->freq, &self->node, OUTPUT);
    if(r != 0) {
	gln_node_destroy(&self->node);
	return r;
    }

    r = smaths_parameter_init(&self->note, &self->node, 0.0f);
    if(r != 0) {
	gln_socket_destroy(&self->freq);
	gln_node_destroy(&self->node);
	return r;
    }

    r = smaths_parameter_init(&self->root, &self->node, smaths_graph_normalized_frequency(self->graph, SMATHS_C));
    if(r != 0) {
	smaths_parameter_destroy(&self->note);
	gln_socket_destroy(&self->freq);
	gln_node_destroy(&self->node);
	return r;
    }

    self->tuning = SMATHS_EQUAL_TUNING;
    spinlock_init(&self->tuning_lock);
    atomic_float_set(&self->last_root, smaths_graph_normalized_frequency(self->graph, SMATHS_C));
    return 0;
}

void smaths_key_destroy(struct smaths_key *self) {
    gln_socket_destroy(&self->freq);
    smaths_parameter_destroy(&self->note);
    smaths_parameter_destroy(&self->root);
    gln_node_destroy(&self->node);
}
