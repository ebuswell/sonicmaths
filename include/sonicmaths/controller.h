/** @file controller.h
 *
 * Controller
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
#ifndef SONICMATHS_CONTROLLER_H
#define SONICMATHS_CONTROLLER_H 1

#include <sonicmaths/graph.h>
#include <graphline.h>

/**
 * Controller
 */
struct smaths_ctlr {
    struct smaths_graph *graph;
    struct gln_node node;
    struct gln_socket out; /** Output */
    struct gln_socket ctl; /** Output control */
};

/**
 * Destroy controller
 */
void smaths_ctlr_destroy(struct smaths_ctlr *ctlr);

/**
 * Initialize controller
 */
int smaths_ctlr_init(struct smaths_ctlr *ctlr, struct smaths_graph *graph, gln_process_fp_t func, void *arg);

#endif /* ! SONICMATHS_CONTROLLER_H */
