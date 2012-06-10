/** @file modulator.h
 *
 * Modulator
 *
 * Multiplies its inputs together.
 */
/*
 * Copyright 2012 Evan Buswell
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
#ifndef SONICMATHS_MODULATOR_H
#define SONICMATHS_MODULATOR_H 1

#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>

/**
 * Modulator
 */
struct smaths_modu {
    struct gln_node node;
    struct smaths_graph *graph;
    struct gln_socket out;
    struct smaths_parameter in1;
    struct smaths_parameter in2;
};

/**
 * Destroy modulator
 */
void smaths_modu_destroy(struct smaths_modu *modu);

/**
 * Initialize modulator
 */
int smaths_modu_init(struct smaths_modu *modu, struct smaths_graph *graph);

#endif
