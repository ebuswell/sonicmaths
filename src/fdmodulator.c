/*
 * fdmodulator.c
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
#include <fftw3.h>
#include <string.h>
#include "sonicmaths/math.h"
#include "sonicmaths/fdmodulator.h"

int smfdmod_init(struct smfdmod *mod, size_t len) {
	unsigned int i;
	mod->R = len / 2;
	mod->cbuf = mod->i = 0;
	for (i = 0; i < 4; i++) {
		mod->abuf[i] = smstft_alloc(len);
		if (mod->abuf[i] == NULL) {
			goto error1;
		}
	}
	for (i = 0; i < 2; i++) {
		mod->bbuf[i] = smstft_alloc(len);
		if (mod->bbuf[i] == NULL) {
			goto error2;
		}
	}
	for (i = 0; i < 4; i++) {
		mod->aplans[i] = smstft_plan_create(mod->abuf[i], len);
		if (mod->aplans[i] == NULL) {
			goto error3;
		}
	}
	for (i = 0; i < 4; i++) {
		mod->inv_plans[i] = smstft_inv_plan_create(mod->abuf[i], len);
		if (mod->inv_plans[i] == NULL) {
			goto error4;
		}
	}
	for (i = 0; i < 2; i++) {
		mod->bplans[i] = smstft_plan_create(mod->bbuf[i], len);
		if (mod->bplans[i] == NULL) {
			goto error5;
		}
	}
	for (i = 0; i < 4; i++) {
		memset(mod->abuf[i], 0, len * sizeof(float));
	}
	for (i = 0; i < 2; i++) {
		memset(mod->bbuf[i], 0, len * sizeof(float));
	}

	return 0;

error5:
	while(i--) {
		smstft_plan_destroy(mod->bplans[i]);
	}
	i = 4;
error4:
	while(i--) {
		smstft_plan_destroy(mod->inv_plans[i]);
	}
	i = 4;
error3:
	while(i--) {
		smstft_plan_destroy(mod->aplans[i]);
	}
	i = 2;
error2:
	while (i--) {
		smstft_free(mod->bbuf[i]);
	}
	i = 4;
error1:
	while (i--) {
		smstft_free(mod->abuf[i]);
	}
	return -1;
}

void smfdmod_destroy(struct smfdmod *mod) {
	unsigned int i;
	for (i = 0; i < 4; i++) {
		smstft_free(mod->abuf[i]);
		smstft_plan_destroy(mod->aplans[i]);
		smstft_plan_destroy(mod->inv_plans[i]);
	}
	for (i = 0; i < 2; i++) {
		smstft_free(mod->bbuf[i]);
		smstft_plan_destroy(mod->bplans[i]);
	}
}
