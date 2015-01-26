/*
 * delay.c
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

#include <stdlib.h>
#include <string.h>
#include "sonicmaths/delay.h"

int smdelay_init(struct smdelay *delay, size_t len) {
	delay->len = len;
	delay->i = 0;
	delay->x = malloc(sizeof(float) * len);
	if (delay->x == NULL) {
		return -1;
	}
	memset(delay->x, 0, sizeof(float) * len);

	return 0;
}

void smdelay_destroy(struct smdelay *delay) {
	free(delay->x);
}
