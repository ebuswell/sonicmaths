/** @file lowpass.h
 *
 * Lowpass filter
 *
 * @verbatim
H(s) = 1 / (s^2 + s/Q + 1)
@endverbatim
 *
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
#ifndef SONICMATHS_LOWPASS_H
#define SONICMATHS_LOWPASS_H 1

#include <sonicmaths/graph.h>
#include <sonicmaths/second-order.h>

/**
 * Lowpass filter
 *
 * See @ref struct smaths_2order
 */
struct smaths_lowpass {
    struct smaths_2order;
};

/**
 * Destroy lowpass filter
 *
 * See @ref smaths_2order_destroy
 */
#define smaths_lowpass_destroy smaths_2order_destroy

/**
 * Initialize lowpass filter
 *
 * See @ref smaths_2order_init
 */
int smaths_lowpass_init(struct smaths_lowpass *lowpass, struct smaths_graph *graph, void (*destroy)(struct smaths_lowpass *));

/**
 * Create lowpass filter
 *
 * Convenience wrapper for `smaths_lowpass_init`
 */
struct smaths_lowpass *smaths_lowpass_create(struct smaths_graph *graph);

#endif /* ! SONICMATHS_LOWPASS_H */
