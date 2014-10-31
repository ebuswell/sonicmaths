/*
 * key.c
 * 
 * Copyright 2014 Evan Buswell
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

#include "sonicmaths/key.h"

struct smkey_western smkey_major = {
    7, {
	1.0,
	9.0/8.0,
	5.0/4.0,
	4.0/3.0,
	3.0/2.0,
	(4.0/3.0)*(5.0/4.0),
	(3.0/2.0)*(5.0/4.0)
    }
};

struct smkey_western smkey_minor = {
    7, {
	1.0,
	9.0/8.0,
	6.0/5.0,
	4.0/3.0,
	3.0/2.0,
	(4.0/3.0)*(6.0/5.0),
	(3.0/2.0)*(6.0/5.0)
    }
};

struct smkey_chromatic smkey_pythagorean = {
    12, {
	1.0,
	256.0/243.0,
	9.0/8.0,
	32.0/27.0,
	81.0/64.0,
	4.0/3.0,
	1024.0/729.0,
	32.0/27.0,
	128.0/81.0,
	27.0/16.0,
	16.0/9.0,
	243.0/128.0
    }
};


