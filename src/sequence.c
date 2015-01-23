/*
 * Copyright 2015 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, version 2.
 * 
 * Sonic Maths is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with Sonic Maths.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <libgen.h>
#include <limits.h>
#include <unistd.h>
#include <sys/inotify.h>
#include "sonicmaths/sequence.h"
#include "sequence.h"

int smseq_lex_init_extra(bool, void **);
int smseq_lex_destroy(void *);
void smseq_set_in(FILE *, void *);
int smseq_parse(struct smseq_parser *);


static void smseq_beats_to_abs(struct smseq_beatlist *beats,
			       float nbeats, float *maxseq) {
	size_t i, j;
	float mcount = 0;
	float next, curr, span;
	float offset;
	float n, d;

	if (beats->len == 0) {
		*maxseq = 0.0f;
		return;
	}

	j = 0;
	offset = curr = beats->beats[0].sequence;

	for (i = 1, d = 1.0f; i < beats->len; i++, d += 1.0f) {
		if ((next = beats->beats[i].sequence) >= 0.0f) {
			span = next > curr ? next - curr
					   : (nbeats - curr) + next;
			for (n = 0.0f; j < i; j++, n += 1.0f) {
				beats->beats[j].sequence
					= mcount * nbeats
					  + curr - offset
					  + (span * n) / d;
			}
			d = 0.0f;
			if (next <= curr) {
				mcount += 1.0f;
			}
			curr = next;
		}
	}

	/* Do the last beat */
	*maxseq = mcount * nbeats
		  + curr - offset + 1.0f;
	for (n = 0.0f; j < i; j++, n += 1.0f) {
		beats->beats[j].sequence
			= mcount * nbeats
			  + curr - offset
			  + n / d;
	}
}

static int smseq_nchannels(struct smseq_beatlist *beats) {
	size_t i;
	size_t nchannels = 0;
	for (i = 0; i < beats->len; i++) {
		if (beats->beats[i].events->len > nchannels) {
			nchannels = beats->beats[i].events->len;
		}
	}
	return (int) nchannels;
}

int smseq_init(struct smseq *seq,
	       char *filename,
	       void (*error)(const char *)) {

	FILE *fp;
	struct smseq_parser seq_info;
	int r;

	seq_info.root = 0.0f;
	seq_info.nbeats = 4.0f;
	seq_info.multiple = 1.0f;
	seq_info.loop = true;
	seq_info.error = error;

	r = smseq_lex_init_extra(false, &seq_info.scanner);
	if (r != 0) {
		return r;
	}

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		smseq_lex_destroy(seq_info.scanner);
		return -1;
	}

	smseq_set_in(fp, seq_info.scanner);
	r = smseq_parse(&seq_info);
	smseq_lex_destroy(seq_info.scanner);
	fclose(fp);
	if (r != 0) {
		return r;
	}

	smseq_beats_to_abs(seq_info.beats, seq_info.nbeats, &seq->maxseq);

	seq->nchannels = smseq_nchannels(seq_info.beats);
	seq->multiple = seq_info.multiple;
	seq->beats = seq_info.beats;
	seq->loop = seq_info.loop;
	seq->prev_time = calloc(seq->nchannels, sizeof(float));
	if (seq->prev_time == NULL) {
		size_t i;
		for (i = 0; i < seq->beats->len; i++) {
			struct smseq_beat *beat;
			beat = &seq->beats->beats[i];
			free(beat->events);
		}
		free(seq->beats);
		return -1;
	}
	seq->prev_value = calloc(seq->nchannels, sizeof(float));
	if (seq->prev_value == NULL) {
		size_t i;
		for (i = 0; i < seq->beats->len; i++) {
			struct smseq_beat *beat;
			beat = &seq->beats->beats[i];
			free(beat->events);
		}
		free(seq->beats);
		free(seq->prev_time);
		return -1;
	}

	return 0;
}

void smseq_destroy(struct smseq *seq) {
	size_t i;
	for (i = 0; i < seq->beats->len; i++) {
		struct smseq_beat *beat;
		beat = &seq->beats->beats[i];
		free(beat->events);
	}
	free(seq->beats);
	free(seq->prev_time);
	free(seq->prev_value);
}
