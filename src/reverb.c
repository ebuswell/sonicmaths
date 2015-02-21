/*
 * reverb.c
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
#include "sonicmaths/reverb.h"
#include "sonicmaths/delay.h"

int smverb_init(struct smverb *verb, size_t delaylen, size_t ndelays) {
	size_t i;
	int r;
	verb->delays = malloc(sizeof(struct smdelay) * ndelays);
	if (verb->delays == NULL) {
		return -1;
	}
	verb->tdist = malloc(sizeof(float) * ndelays);
	if (verb->tdist == NULL) {
		free(verb->delays);
		return -1;
	}
	for (i = 0; i < ndelays; i++) {
		r = smdelay_init(&verb->delays[i], delaylen);
		if (r != 0) {
			while (i--) {
				smdelay_destroy(&verb->delays[i]);
			}
			free(verb->delays);
			free(verb->tdist);
			return r;
		}
		verb->tdist[i] = smrand_gaussian();
	}
	return 0;
}

void smverb_destroy(struct smverb *verb) {
	size_t i;
	for (i = 0; i < verb->ndelays; i++) {
		smdelay_destroy(&verb->delays[i]);
	}
	free(verb->delays);
	free(verb->tdist);
}
