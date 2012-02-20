/** @file notch.h
 *
 * Notch filter
 *
 * Ruby version: @c Filters::Notch
 *
 * @verbatim
H(s) = (s^2 + 1) / (s^2 + s/Q + 1)
@endverbatim
 *
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
#ifndef SONICMATHS_NOTCH_H
#define SONICMATHS_NOTCH_H 1

#include <sonicmaths/lowpass.h>

/**
 * Notch filter
 *
 * See @ref struct smaths_lowpass
 */
struct smaths_notch {
    struct smaths_lowpass lowpass;
};

/**
 * Destroy notch filter
 *
 * See @ref cs_filter_destroy
 */
inline void smaths_notch_destroy(struct smaths_notch *notch) {
    smaths_lowpass_destroy(&notch->lowpass);
}

/**
 * Initialize notch filter
 *
 * See @ref cs_filter_init
 */
int smaths_notch_init(struct smaths_notch *notch, struct smaths_graph *graph);

#endif /* ! SONICMATHS_NOTCH_H */
