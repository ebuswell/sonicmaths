/** @file highpass.h
 *
 * Highpass filter
 *
 * @verbatim
H(s) = s^2 / (s^2 + s/Q + 1)
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
#ifndef SONICMATHS_HIGHPASS_H
#define SONICMATHS_HIGHPASS_H 1

#include <sonicmaths/graph.h>
#include <sonicmaths/second-order.h>

/**
 * Highpass filter
 *
 * See @ref struct smaths_2order
 */
struct smaths_highpass {
    struct smaths_2order;
};

/**
 * Destroy highpass filter
 *
 * See @ref smaths_2order_destroy
 */
#define smaths_highpass_destroy smaths_2order_destroy

/**
 * Initialize highpass filter
 *
 * See @ref smaths_2order_init
 */
int smaths_highpass_init(struct smaths_highpass *highpass, struct smaths_graph *graph, void (*destroy)(struct smaths_highpass *));

/**
 * Create highpass filter
 *
 * Convenience wrapper for `smaths_highpass_init`
 */
struct smaths_highpass *smaths_highpass_create(struct smaths_graph *graph);

#endif /* ! SONICMATHS_HIGHPASS_H */
