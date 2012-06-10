/** @file clock.h
 * Clock
 *
 * The clock outputs a timestamp for each sample.
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
    struct gln_node node; /** Node for this clock */
    struct smaths_graph *graph; /** Graph for this clock */
    struct gln_socket clock; /** Output clock port */
    struct smaths_parameter rate; /** Rate at which this clock progresses */
    double current; /** The current value */
};

/**
 * Initialize clock
 */
int smaths_clock_init(struct smaths_clock *clock, struct smaths_graph *graph);

/**
 * Destroy clock
 */
void smaths_clock_destroy(struct smaths_clock *clock);

#endif /* ! SONICMATHS_CLOCK_H */
