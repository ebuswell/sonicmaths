/*
 * key.c
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

#include <stddef.h>
#include <math.h>
#include <atomickit/atomic-rcp.h>
#include <atomickit/atomic-malloc.h>
#include <graphline.h>
#include "sonicmaths/buffer.h"
#include "sonicmaths/graph.h"
#include "sonicmaths/parameter.h"
#include "sonicmaths/key.h"

struct smaths_key_tuning_western smaths_major_tuning = {
    ARCP_REGION_VAR_INIT(0, 1, NULL, NULL),
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

struct smaths_key_tuning_western smaths_minor_tuning = {
    ARCP_REGION_VAR_INIT(0, 1, NULL, NULL),
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

struct smaths_key_tuning_chromatic smaths_pythagorean_tuning = {
    ARCP_REGION_VAR_INIT(0, 1, NULL, NULL),
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
	if(f != 0) {
	    if(m == (tuning_length - 1)) {
		freq *= powf(2/tuning->tuning[m], f);
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

static int smaths_key_process(struct smaths_key *key) {
    int r, i, j;

    struct smaths_buffer *note_buffer;
    struct smaths_buffer *root_buffer;

    r = gln_get_buffers(2, key->note, &note_buffer, key->root, &root_buffer);
    if(r != 0) {
	return r;
    }

    float note = smaths_parameter_go(key->note, note_buffer);
    float root = smaths_parameter_go(key->root, root_buffer);

    int nframes = smaths_node_frames_per_period(key);
    if(nframes < 0) {
	return nframes;
    }

    int nchannels = SMATHS_MAX_NCHANNELS(2, note_buffer, root_buffer);

    struct smaths_buffer *freq_buffer = smaths_alloc_buffer(key->freq, nframes, nchannels);
    if(freq_buffer == NULL) {
	return -1;
    }

    struct smaths_key_tuning *tuning = (struct smaths_key_tuning *) arcp_load(&key->tuning);

    for(i = 0; i < nframes; i++) {
	for(j = 0; j < nchannels; j++) {
	    freq_buffer->data[i * nchannels + j]
		= smaths_key_note2freq_param(smaths_value(note_buffer, i, j, note),
					     smaths_value(root_buffer, i, j, root),
					     tuning);
	}
    }

    arcp_release(tuning);

    return 0;
}

float smaths_key_note2freq(struct smaths_key *key, float note) {
    float root = smaths_parameter_get(key->root);
    struct smaths_key_tuning *tuning = (struct smaths_key_tuning *) arcp_load(&key->tuning);
    float ret = smaths_key_note2freq_param(note, root, tuning);
    arcp_release(tuning);
    return ret;
}

int smaths_key_init(struct smaths_key *key, struct smaths_graph *graph, void (*destroy)(struct smaths_key *)) {
    int r;

    r = gln_node_init(key, graph, (gln_process_fp_t) smaths_key_process, (void (*)(struct gln_node *)) destroy);
    if(r != 0) {
	goto undo0;
    }

    key->freq = gln_socket_create(key, GLNS_OUTPUT);
    if(key->freq == NULL) {
	r = -1;
	goto undo1;
    }

    key->note = smaths_parameter_create(key, 0.0f);
    if(key->note == NULL) {
	r = -1;
	goto undo2;
    }

    key->root = smaths_parameter_create(key, smaths_graph_normalized_frequency(graph, SMATHS_C));
    if(key->root == NULL) {
	r = -1;
	goto undo3;
    }

    arcp_init(&key->tuning, SMATHS_EQUAL_TUNING);
    return 0;

undo3:
    arcp_release(key->note);
undo2:
    arcp_release(key->freq);
undo1:
    gln_node_destroy(key);
undo0:
    return r;
}

void smaths_key_destroy(struct smaths_key *key) {
    arcp_store(&key->tuning, NULL);
    arcp_release(key->root);
    arcp_release(key->note);
    arcp_release(key->freq);
    gln_node_destroy(key);
}

static void __smaths_key_destroy(struct smaths_key *key) {
    smaths_key_destroy(key);
    afree(key, sizeof(struct smaths_key));
}

struct smaths_key *smaths_key_create(struct smaths_graph *graph) {
    int r;
    struct smaths_key *ret;

    ret = amalloc(sizeof(struct smaths_key));
    if(ret == NULL) {
	return NULL;
    }

    r = smaths_key_init(ret, graph, __smaths_key_destroy);
    if(r != 0) {
	return NULL;
    }

    return ret;
}
