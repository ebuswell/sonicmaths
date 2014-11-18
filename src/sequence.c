/*
 * Copyright 2014 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, version 2.
 * 
 * Sonic Maths is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * Sonic Maths.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <atomickit/malloc.h>
#include <atomickit/rcp.h>
#include "sonicmaths/sequence.h"
#include "sequence.h"

int smseq_lex_init_extra(bool, void **);
int smseq_lex_destroy(void *);
void smseq_set_in(FILE *, void *);
int smseq_parse(struct smseq_parser *);


static void smseq_beats_to_abs(struct smseq_beatlist *beats, float nbeats) {
	size_t i, j;
	float mcount = 0;
	float next, curr, span;
	float offset;
	float n, d;

	if(beats->len == 0) {
		return;
	}

	j = 0;
	offset = curr = beats->beats[0].sequence;

	for(i = 1, d = 1.0f; i < beats->len; i++, d += 1.0f) {
		if((next = beats->beats[i].sequence) >= 0.0f) {
			span = next > curr ? next - curr
			                   : (nbeats - curr) + next;
			for(n = 0.0f; j < i; j++, n += 1.0f) {
				beats->beats[j].sequence
					= mcount * nbeats
					  + curr - offset
					  + (span * n) / d;
			}
			d = 0.0f;
			if(next <= curr) {
				mcount += 1.0f;
			}
			curr = next;
		}
	}

	/* Do the last beat */
	for(n = 0.0f; j < i; j++, n += 1.0f) {
		beats->beats[j].sequence
			= mcount * nbeats
			  + curr - offset
			  + n / d;
	}
}

static int smseq_nchannels(struct smseq_beatlist *beats) {
	size_t i;
	size_t nchannels = 0;
	for(i = 0; i < beats->len; i++) {
		if(beats->beats[i].events->len > nchannels) {
			nchannels = beats->beats[i].events->len;
		}
	}
	return (int) nchannels;
}

int smseq_init(struct smseq *seq,
               char *filename,
	       void (*error)(const char *),
               void (*destroy)(struct smseq *seq)) {

	FILE *fp;
	struct smseq_parser seq_info;
	int r;

	seq_info.root = 0.0f;
	seq_info.nbeats = 4.0f;
	seq_info.multiple = 1.0f;
	seq_info.error = error;

	r = smseq_lex_init_extra(false, &seq_info.scanner);
	if(r != 0) {
		return r;
	}

	fp = fopen(filename, "rb");
	if(fp == NULL) {
		smseq_lex_destroy(seq_info.scanner);
		return -1;
	}

	smseq_set_in(fp, seq_info.scanner);
	r = smseq_parse(&seq_info);
	smseq_lex_destroy(seq_info.scanner);
	fclose(fp);
	if(r != 0) {
		return r;
	}

	smseq_beats_to_abs(seq_info.beats, seq_info.nbeats);

	seq->nchannels = smseq_nchannels(seq_info.beats);
	seq->multiple = seq_info.multiple;
	seq->beats = seq_info.beats;
	seq->prev_time = amalloc(sizeof(float) * seq->nchannels);
	if(seq->prev_time == NULL) {
		size_t i;
		for(i = 0; i < seq->beats->len; i++) {
			struct smseq_beat *beat;
			beat = &seq->beats->beats[i];
			afree(beat->events, sizeof(struct smseq_eventlist)
		                    	    + sizeof(struct smseq_event)
		                      	      * beat->events->len);
		}
		afree(seq->beats, sizeof(struct smseq_beatlist)
	                  	  + sizeof(struct smseq_beat)
	                    	    * seq->beats->len);
	        return -1;
	}
	memset(seq->prev_time, 0, sizeof(float) * seq->nchannels);
	seq->prev_value = amalloc(sizeof(float) * seq->nchannels);
	if(seq->prev_value == NULL) {
		size_t i;
		for(i = 0; i < seq->beats->len; i++) {
			struct smseq_beat *beat;
			beat = &seq->beats->beats[i];
			afree(beat->events, sizeof(struct smseq_eventlist)
		                    	    + sizeof(struct smseq_event)
		                      	      * beat->events->len);
		}
		afree(seq->beats, sizeof(struct smseq_beatlist)
	                  	  + sizeof(struct smseq_beat)
	                    	    * seq->beats->len);
		afree(seq->prev_time, sizeof(float) * seq->nchannels);
		return -1;
	}
	memset(seq->prev_value, 0, sizeof(float) * seq->nchannels);

	arcp_region_init(seq, (arcp_destroy_f) destroy);

	return 0;
}

void smseq_destroy(struct smseq *seq) {
	size_t i;
	for(i = 0; i < seq->beats->len; i++) {
		struct smseq_beat *beat;
		beat = &seq->beats->beats[i];
		afree(beat->events, sizeof(struct smseq_eventlist)
		                    + sizeof(struct smseq_event)
		                      * beat->events->len);
	}
	afree(seq->beats, sizeof(struct smseq_beatlist)
	                  + sizeof(struct smseq_beat)
	                    * seq->beats->len);
	afree(seq->prev_time, sizeof(float) * seq->nchannels);
	afree(seq->prev_value, sizeof(float) * seq->nchannels);
}

static void __smseq_destroy(struct smseq *seq) {
	smseq_destroy(seq);
	afree(seq, sizeof(struct smseq));
}

struct smseq *smseq_create(char *filename, void (*error)(const char *)) {
	int r;
	struct smseq *seq;

	seq = amalloc(sizeof(struct smseq));
	if(seq == NULL) {
		return NULL;
	}

	r = smseq_init(seq, filename, error, __smseq_destroy);
	if(r != 0) {
		afree(seq, sizeof(struct smseq));
		return NULL;
	}

	return seq;
}


