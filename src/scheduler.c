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

#include <math.h>
#include <atomickit/atomic-list.h>
#include <graphline.h>
#include "sonicmaths/graph.h"
#include "sonicmaths/scheduler.h"

static int event_compar(struct smaths_sched_event *a, struct smaths_sched_event *b) {
    if(isnanf(a->time)) {
	if(isnanf(b->time)) {
	    return 0;
	} else {
	    return 1;
	}
    } else if(isnanf(b->time)) {
	return -1;
    }
    float res = a->time - b->time;
    return res == 0.0f ? 0
	: res < 0.0f ? -1
	: 1;
}

static int smaths_sched_process(struct smaths_sched *self) {
    float *clock_buffer = gln_socket_get_buffer(&self->clock);
    struct smaths_sched_event *event = atomic_list_first(&self->schedule);
    if(event == ALST_EMPTY) {
	return 0;
    }
    size_t offset;
    for(offset = 0; offset < self->graph->graph.buffer_nmemb; offset++) {
	if(event->time <= clock_buffer[offset]) {
	    struct smaths_sched_event *event2 = atomic_list_shift(&self->schedule);
	    if(event2 != event) {
		/* replace and loop */
		event = event2;
		atomic_list_insert_sorted(&self->schedule, (int(*)(void*,void*)) event_compar, event);
		offset--;
		continue;
	    }
	    int r = event->event_fp(event->arg);
	    if(r != 0) {
		return r;
	    }
	    event = atomic_list_first(&self->schedule);
	    if(event == ALST_EMPTY) {
		return 0;
	    }
	}
    }
    return 0;
}

int smaths_sched_schedule(struct smaths_sched *self, struct smaths_sched_event *event) {
    return atomic_list_insert_sorted(&self->schedule, (int(*)(void*,void*))event_compar, event);
}

void smaths_sched_cancel(struct smaths_sched *self, struct smaths_sched_event *event) {
    atomic_list_remove_by_value(&self->schedule, event);
}

int smaths_sched_init(struct smaths_sched *self, struct smaths_graph *graph) {
    self->graph = graph;
    int r;
    r = atomic_list_init(&self->schedule);
    if(r != 0) {
	return r;
    }
    r = gln_node_init(&self->node, &self->graph->graph, (gln_process_fp_t) smaths_sched_process, self);
    if(r != 0) {
	atomic_list_destroy(&self->schedule);
	return r;
    }
    r = gln_socket_init(&self->clock, &self->node, INPUT);
    if(r != 0) {
	gln_node_destroy(&self->node);
	atomic_list_destroy(&self->schedule);
	return r;
    }
    self->process_closure.func = (smaths_graph_process_fp_t) smaths_sched_process;
    self->process_closure.arg = self;
    r = atomic_list_push(&self->graph->run_first, &self->process_closure);
    if(r != 0) {
	gln_socket_destroy(&self->clock);
	gln_node_destroy(&self->node);
	atomic_list_destroy(&self->schedule);
    }
    return 0;
}

void smaths_sched_destroy(struct smaths_sched *self) {
    atomic_list_remove_by_value(&self->graph->run_first, &self->process_closure);
    gln_socket_destroy(&self->clock);
    gln_node_destroy(&self->node);
    atomic_list_destroy(&self->schedule);
}
