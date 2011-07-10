/** @file scheduler.h
 *
 * Defines a simple scheduler that will schedule a callback function
 * at a certain time.
 *
 */
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
#ifndef SONICMATHS_SCHEDULER_H
#define SONICMATHS_SCHEDULER_H 1

#include <sonicmaths/graph.h>
#include <graphline.h>
#include <atomickit/atomic-list.h>

struct smaths_sched {
    struct smaths_graph *graph; /** Graph for this scheduler */
    struct gln_node node; /** Node for this scheduler */
    struct gln_socket clock; /** The source for timing information */
    atomic_list_t schedule; /** Our event schedule */
    struct smaths_graph_process_closure process_closure; /** Process closure to pass to run_first */
};

typedef int (*smaths_event_fp_t)(void *);

struct smaths_sched_event {
    float time;
    smaths_event_fp_t event_fp;
    void *arg;
};

int smaths_sched_init(struct smaths_sched *sched, struct smaths_graph *graph);
void smaths_sched_destroy(struct smaths_sched *sched);

int smaths_sched_schedule(struct smaths_sched *sched, struct smaths_sched_event *event);
void smaths_sched_cancel(struct smaths_sched *sched, struct smaths_sched_event *event);

#endif /* ! SONICMATHS_SCHEDULER_H */
