/** @file random.h
 *
 * Random numbers
 *
 */
/*
 * Copyright 2015 Evan Buswell
 *
 * Copied with minimal changes from
 * https://github.com/divfor/mt_rand/blob/master/mtrand.h
 *
 * Copyright 2007-2009 The OpenTyrian Development Team
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
#ifndef SONICMATHS_RANDOM_H
#define SONICMATHS_RANDOM_H 1

#include <stdint.h>

#define SMRAND_MAX 0xffffffffUL

void smrand_seed(uint32_t s);
uint32_t smrand(void);
float smrand_uniform(void);
float smrand_gaussian(void);

#endif /* SONICMATHS_RANDOM_H */
