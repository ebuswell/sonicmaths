/*
 * Copied with minimal changes from
 * https://github.com/divfor/mt_rand/blob/master/mtrand.h
 *
 * Copyright (C) 2007-2009 The OpenTyrian Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef MTRAND_H
#define MTRAND_H

#include <stdint.h>

#define MT_RAND_MAX 0xffffffffUL

void mt_srand(uint32_t s);
uint32_t mt_rand(void);
float mt_rand_float(void);

#endif /* MTRAND_H */
