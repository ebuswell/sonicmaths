/** @file impulse-train.h
 *
 * Impulse Train Synth
 *
 * This produces an impulse train, which corresponds to:
 *
 * @verbatim
inf
 Σ cos(nwt)
n=1
@endverbatim
 *
 */
/*
 * Copyright 2015 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, version 2.
 * 
 * Sonic Maths is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with Sonic Maths.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SONICMATHS_IMPULSE_TRAIN_H
#define SONICMATHS_IMPULSE_TRAIN_H 1

#include <sonicmaths/oscillator.h>

void smitrain(struct smosc *osc, int n, float *y, float *f, float *phi);

#endif /* ! SONICMATHS_IMPULSE_TRAIN */
