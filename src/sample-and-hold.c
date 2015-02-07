/*
 * sample-and-hold.c
 * 
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
#include <string.h>
#include "sonicmaths/sample-and-hold.h"

int smsandh_init(struct smsandh *sandh) {
	memset(sandh, 0, sizeof(struct smsandh));
	return 0;
}

void smsandh_destroy(struct smsandh *sandh __attribute__((unused))) {
	/* Do nothing */
}
