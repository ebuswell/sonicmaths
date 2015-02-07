/*
 * shifter.c
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
#include "sonicmaths/math.h"
#include "sonicmaths/oscillator.h"
#include "sonicmaths/second-order.h"
#include "sonicmaths/shifter.h"

int smshift_init(struct smshift *shift) {
	return smosc_init(&shift->osc)
	       | smhilbert_pair_init(&shift->coeff)
	       | sm2order_init(&shift->filter);
}

void smshift_destroy(struct smshift *shift) {
	smhilbert_pair_destroy(&shift->coeff);
	smosc_destroy(&shift->osc);
	sm2order_destroy(&shift->filter);
}
