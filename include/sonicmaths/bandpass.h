/** @file bandpass.h
 *
 * Bandpass filter
 *
 * @verbatim
H(s) = (s/Q) / (s^2 + s/Q + 1)
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
#ifndef SONICMATHS_BANDPASS_H
#define SONICMATHS_BANDPASS_H 1

#include <sonicmaths/graph.h>
#include <sonicmaths/second-order.h>

/**
 * Bandpass filter
 *
 * See @ref struct smaths_2order
 */
struct smaths_bandpass {
    struct smaths_2order;
};

/**
 * Destroy bandpass filter
 *
 * See @ref smaths_2order_destroy
 */
#define smaths_bandpass_destroy smaths_2order_destroy

/**
 * Initialize bandpass filter
 *
 * See @ref smaths_2order_init
 */
int smaths_bandpass_init(struct smaths_bandpass *bandpass, struct smaths_graph *graph, void (*destroy)(struct smaths_bandpass *));

/**
 * Create bandpass filter
 *
 * Convenience wrapper for `smaths_bandpass_init`
 */
struct smaths_bandpass *smaths_bandpass_create(struct smaths_graph *graph);

#endif /* ! SONICMATHS_BANDPASS_H */
