/** @file sequence.h
 *
 * Sequencer
 *
 */
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
#ifndef SONICMATHS_SEQUENCE_H
#define SONICMATHS_SEQUENCE_H 1

#include <pthread.h>
#include <atomickit/atomic.h>
#include <atomickit/rcp.h>

struct smseq_event {
	float value;
	float ctl;
};

struct smseq_eventlist {
	size_t len;
	struct smseq_event events[];
};

struct smseq_beat {
	float sequence;
	struct smseq_eventlist *events;
};

struct smseq_beatlist {
	size_t len;
	struct smseq_beat beats[];
};

struct smseq {
	struct arcp_region;
	int nchannels;
	float multiple;
	bool loop;
	float maxseq;
	struct smseq_beatlist *beats;
	float *prev_time;
	float *prev_value;
};

int smseq_init(struct smseq *seq,
               char *filename,
	       void (*error)(const char *),
               void (*destroy)(struct smseq *seq));

void smseq_destroy(struct smseq *seq);

struct smseq *smseq_create(char *filename, void (*error)(const char *));

pthread_t smseq_start_watch(char *filename, arcp_t *dest,
                            void (*error)(const char *));

static inline float smseq_do(struct smseq *seq, int channel, float value,
                             float prev_time, float time, float *ctl) {
	ssize_t u, l, i;
	bool desc;

	// printf("value: %f\ntime: %f\nprev: %f\n\n", (double) value, (double) time, (double) prev_time);

	desc = (time < prev_time);

	/* Find first event */
	i = l = 0;
	u = seq->beats->len;
	while(l < u) {
		float v;
		i = (l + u) / 2;
		v = seq->beats->beats[i].sequence;
		if(prev_time < v) {
			u = i;
		} else if(prev_time > v) {
			l = ++i;
		} else {
			goto exact_match;
		}
	}

	/* At this point, the event at i is after prev_time, which is fine for
 	 * ascending, but for descending, we want the event at i to be before
 	 * prev_time. Since we didn't get an exact match, this is just the
 	 * previous event. */
	if(desc) {
		i--;
	}

exact_match:

	for(; desc ? (i >= 0) : (i < (ssize_t) seq->beats->len); i += desc ? -1 : 1) {
		struct smseq_event *event;
		if(desc ? (seq->beats->beats[i].sequence < time)
 		        : (seq->beats->beats[i].sequence > time)) {
			break;
		}
		if((int) seq->beats->beats[i].events->len <= channel) {
			continue;
		}
		event = &seq->beats->beats[i].events->events[channel];
		if(event->ctl == 0.0f) {
			continue;
		}
		if(ctl != NULL) {
			*ctl = event->ctl;
		}
		if(event->ctl == 1.0f) {
			value = event->value;
		}
	}
	return value;
}

static inline float smseq(struct smseq *seq, int channel,
                          float time, float *ctl) {
	float prev_time;
	float value;

	if(ctl != NULL) {
		*ctl = 0.0f;
	}

	if(channel >= seq->nchannels) {
		return 0.0f;
	}
	
	value = seq->prev_value[channel];

	time *= seq->multiple;

	prev_time = seq->prev_time[channel];
	seq->prev_time[channel] = time;

	if(seq->loop) {
		while(time > seq->maxseq) {
			time -= seq->maxseq;
			prev_time -= seq->maxseq;
		}
		while(time < 0.0f) {
			time += seq->maxseq;
			prev_time += seq->maxseq;
		}
		if(prev_time < 0.0f) {
			/* we just looped while ascending */
			value = smseq_do(seq, channel, value,
			                 seq->maxseq + prev_time,
			                 seq->maxseq,
			                 ctl);
			value = smseq_do(seq, channel, value,
			                 0.0f,
			                 time,
			                 ctl);
			goto done;
		}
		if(prev_time > seq->maxseq) {
			/* we just looped while descending */
			value = smseq_do(seq, channel, value,
			                 prev_time - seq->maxseq,
			                 0.0f,
			                 ctl);
			value = smseq_do(seq, channel, value,
			                 seq->maxseq,
			                 time,
			                 ctl);
			goto done;
		}
	}
	value = smseq_do(seq, channel, value,
			 prev_time,
			 time,
			 ctl);
done:
	seq->prev_value[channel] = value;
	return value;
}

#endif /* ! SONICMATHS_SEQUENCE_H */
