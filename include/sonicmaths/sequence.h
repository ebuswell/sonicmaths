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
	struct smseq_beatlist *beats;
	float *prev_time;
	float *prev_value;
};

static inline float smseq(struct smseq *seq, int channel, float time, float *ctl) {
	int u, l, i, j;
	bool exact, prev_exact;
	if(ctl != NULL) {
		*ctl = 0.0f;
	}
	if(channel >= seq->nchannels) {
		return 0.0f;
	}
	/* find the previous event using binary search */
	l = 0;
	u = seq->beats->len;
	while(l < u) {
		float v, needle;
		i = (l + u) / 2;
		v = seq->beats->beats[i].sequence;
		needle = seq->prev_time[channel];
		if(needle < v) {
			u = i;
		} else if(needle > v) {
			l = ++i;
		} else {
			prev_exact = true;
			break;
		}
	}
	if(time < seq->prev_time[channel]) {
		/* Moving down */
		u = i + 1;
		l = 0;
	} else {
		/* Moving up */
		l = i;
		u = seq->beats->len;
	}
	while(l < u) {
		float v;
		j = (l + u) / 2;
		v = seq->beats->beats[j].sequence;
		if(time < v) {
			u = j;
		} else if(time > v) {
			l = ++j;
		} else {
			exact = true;
			break;
		}
	}
	seq->prev_time[channel] = time;
	if(time < seq->prev_time[channel]) {
		/* Moving down */
		u = i;
		l = exact ? j - 1 : j;
		for(i = u; i > l; i--) {
			struct smseq_event *event;
			if((int) seq->beats->beats[i].events->len <= channel) {
				continue;
			}
			event = &seq->beats->beats[i].events->events[channel];
			if(event->ctl == 0) {
				continue;
			}
			if(ctl != NULL) {
				*ctl = event->ctl;
			}
			seq->prev_value[channel] = event->value;
		}
	} else {
		/* Moving up */
		l = prev_exact ? i : i + 1;
		u = exact ? j : j + 1;
		for(i = l; i < u; i++) {
			struct smseq_event *event;
			if((int) seq->beats->beats[i].events->len <= channel) {
				continue;
			}
			event = &seq->beats->beats[i].events->events[channel];
			if(event->ctl == 0) {
				continue;
			}
			if(ctl != NULL) {
				*ctl = event->ctl;
			}
			seq->prev_value[channel] = event->value;
		}
	}
	return seq->prev_value[channel];
}

#endif /* ! SONICMATHS_SEQUENCE_H */
