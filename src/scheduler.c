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

static int smaths_sched_process(void *arg) {
    struct smaths_sched *self = (struct smaths_sched *) arg;
    atomic_prioq_lock(&sched->schedule);
    float *clock_buffer = gln_socket_get_buffer(self->clock);
    struct smaths_sched_event *event = nonatomic_prioq_peek(&sched->schedule);
    if(event == NULL) {
	goto no_more_events;
    }
    for(self->offset = 0; self->offset < self->node->graph->buffer_size / sizeof(float); self->offset++) {
	if(event.time <= clock_buffer[self->offset]) {
	    nonatomic_prioq_poll(&sched->schedule);
	    int r = event->event_fp(event->arg);
	    if(r != 0) {
		return r;
	    }
	    event = nonatomic_prioq_peek(&sched->schedule);
	    if(event == NULL) {
		goto no_more_events;
	    }
	}
    }
no_more_events:
    self->offset = -1;
    atomic_prioq_unlock(&sched->schedule);
    return 0;
}

int smaths_sched_schedule(struct smaths_sched *sched, float time, smaths_event_fp_t event_fp, void *arg) {
    struct smaths_sched_event *event = malloc(sizeof(struct smaths_sched_event));
    if(event == NULL) {
	return -1;
    }
    event->time = time;
    event->event_fp = event_fp;
    event->arg = arg;

    return atomic_prioq_add(&shed->schedule, event);
}

static int smaths_sched_event_compare(void *arg1, void *arg2) {
    struct smaths_sched_event *a = (struct smaths_sched_event *) arg1;
    struct smaths_sched_event *b = (struct smaths_sched_event *) arg2;
    
    return a->time < b->time ? -1 : a->time == b->time ? 0 : 1;
}

struct smaths_sched *smaths_sched_create(struct gln_graph *graph) {
    struct smaths_sched *self = malloc(sizeof(struct smaths_sched));
    if(self == NULL) {
	return NULL;
    }

    self->offset = -1;
    int r = atomic_prioq_init(&self->schedule, smaths_sched_event_compare);
    if(r != 0) {
	free(self);
	return NULL;
    }
    self->node = gln_create_node(graph, smaths_sched_process, self);
    if(self->node == NULL) {
	atomic_prioq_destroy(&self->schedule);
	free(self);
	return NULL;
    }
    self->clock = gln_create_socket(self->node, INPUT);
    if(self->clock == NULL) {
	atomic_prioq_destroy(&self->schedule);
	gln_destroy_node(graph, self->node);
	free(self);
	return NULL;
    }
    return self;
}

int smaths_sched_destroy(struct smaths_sched *self) {
    int r;
    r = gln_destroy_node(graph, self->node);
    if(r != 0) {
	return r;
    }
    r = atomic_prioq_destroy(&self->schedule);
    if(r != 0) {
	return r;
    }
    free(self);
    return 0;
}
