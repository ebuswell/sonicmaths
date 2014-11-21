/** @file noise.h
 *
 * Noise Generator
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
#ifndef SONICMATHS_NOISE_H
#define SONICMATHS_NOISE_H 1

#include <atomickit/rcp.h>

enum smnoise_kind {
	SMNOISE_GAUSSIAN = 0,
	SMNOISE_UNIFORM
};

float smnoise(enum smnoise_kind kind);

#endif

