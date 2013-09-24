/** @file clock.h
 * Clock
 *
 * The clock outputs a timestamp for each sample.
 */
/*
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
#ifndef SONICMATHS_CLOCK_H
#define SONICMATHS_CLOCK_H 1

#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>

/**
 * Clock
 *
 * The clock outputs a timestamp for each sample.
 */
struct smaths_clock {
    struct gln_node; /** Node for this clock */
    struct gln_socket *clock; /** Output clock socket */
    struct smaths_parameter *rate; /** Rate at which this clock progresses */
    int nchannels; /** The number of channels of state we're currently storing. */
    float *t; /** The current time */
};

/**
 * Initialize clock
 */
int smaths_clock_init(struct smaths_clock *clock, struct smaths_graph *graph, void (*destroy)(struct smaths_clock *clock));

/**
 * Create clock
 */
struct smaths_clock *smaths_clock_create(struct smaths_graph *graph);

/**
 * Destroy clock
 */
void smaths_clock_destroy(struct smaths_clock *clock);

#endif /* ! SONICMATHS_CLOCK_H */
