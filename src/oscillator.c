/*
 * oscillator.c
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
#include "sonicmaths/random.h"
#include "sonicmaths/oscillator.h"

int smosc_init(struct smosc *osc) {
	osc->t = (smrand_uniformv() + 1.0f) / 2.0f;
	return 0;
}

void smosc_destroy(struct smosc *osc __attribute__((unused))) {
	/* Do nothing */
}
