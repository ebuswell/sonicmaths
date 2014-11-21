/*
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

#include <atomickit/rcp.h>
#include <atomickit/malloc.h>
#include "sonicmaths/synth.h"
#include "sonicmaths/sine.h"
#include "sonicmaths/impulse-train.h"
#include "sonicmaths/integrator.h"
#include "sonicmaths/second-order.h"
#include "sonicmaths/envelope-generator.h"
#include "sonicmaths/bandpass.h"
#include "sonicmaths/lowpass.h"
#include "sonicmaths/predefs.h"

void smasynth_destroy(struct smasynth *asynth) {
	int i;
	smsynth_destroy(&asynth->lfo);
	sm2order_destroy(&asynth->filterres);
	sm2order_destroy(&asynth->filterlp);
	smenvg_destroy(&asynth->fenvg);
	smenvg_destroy(&asynth->aenvg);
	i = SMASYNTH_NINTGS;
	for(i--; i >= 0; i--) {
		smintg_destroy(&asynth->intg[i]);
	}
	i = SMASYNTH_NSYNTHS;
	for(i--; i >= 0; i--) {
		smsynth_destroy(&asynth->synth[i]);
	}
}

static void __smasynth_destroy(struct smasynth *asynth) {
	smasynth_destroy(asynth);
	afree(asynth, sizeof(struct smasynth));
}

int smasynth_init(struct smasynth *asynth,
                  void (*destroy)(struct smasynth *)) {
	int r, i;

	for(i = 0; i < SMASYNTH_NSYNTHS; i++) {
		r = smsynth_init(&asynth->synth[i], NULL);
		if(r != 0) {
			goto undo1;
		}
	}

	for(i = 0; i < SMASYNTH_NINTGS; i++) {
		r = smintg_init(&asynth->intg[i], NULL);
		if(r != 0) {
			goto undo2;
		}
	}

	r = smenvg_init(&asynth->aenvg, NULL);
	if(r != 0) {
		goto undo3;
	}

	r = smenvg_init(&asynth->fenvg, NULL);
	if(r != 0) {
		goto undo4;
	}

	r = sm2order_init(&asynth->filterlp, NULL);
	if(r != 0) {
		goto undo5;
	}

	r = sm2order_init(&asynth->filterres, NULL);
	if(r != 0) {
		goto undo6;
	}

	r = smsynth_init(&asynth->lfo, NULL);
	if(r != 0) {
		goto undo7;
	}

	arcp_region_init(asynth, (arcp_destroy_f) destroy);

	return 0;
undo7:
	sm2order_destroy(&asynth->filterres);
undo6:
	sm2order_destroy(&asynth->filterlp);
undo5:
	smenvg_destroy(&asynth->fenvg);
undo4:
	smenvg_destroy(&asynth->aenvg);
undo3:
	i = SMASYNTH_NINTGS;
undo2:
	for(i--; i >= 0; i--) {
		smintg_destroy(&asynth->intg[i]);
	}
	i = SMASYNTH_NSYNTHS;
undo1:
	for(i--; i >= 0; i--) {
		smsynth_destroy(&asynth->synth[i]);
	}
	return r;
}

struct smasynth *smasynth_create() {
	struct smasynth *asynth;
	int r;

	asynth = amalloc(sizeof(struct smasynth));
	if(asynth == NULL) {
		return NULL;
	}

	r = smasynth_init(asynth, __smasynth_destroy);
	if(r != 0) {
		afree(asynth, sizeof(struct smasynth));
		return NULL;
	}

	return asynth;
}

void smdrum_destroy(struct smdrum *drum) {
	sm2order_destroy(&drum->filterlp);
	smenvg_destroy(&drum->pitchenvg);
	smenvg_destroy(&drum->noiseenvg);
	smenvg_destroy(&drum->toneenvg);
	smsynth_destroy(&drum->osc);
}

static void __smdrum_destroy(struct smdrum *drum) {
	smdrum_destroy(drum);
	afree(drum, sizeof(struct smdrum));
}

int smdrum_init(struct smdrum *drum,
                  void (*destroy)(struct smdrum *)) {
	int r;

	r = smsynth_init(&drum->osc, NULL);
	if(r != 0) {
		goto undo0;
	}

	r = smenvg_init(&drum->toneenvg, NULL);
	if(r != 0) {
		goto undo1;
	}

	r = smenvg_init(&drum->noiseenvg, NULL);
	if(r != 0) {
		goto undo2;
	}

	r = smenvg_init(&drum->pitchenvg, NULL);
	if(r != 0) {
		goto undo3;
	}

	r = sm2order_init(&drum->filterlp, NULL);
	if(r != 0) {
		goto undo4;
	}

	arcp_region_init(drum, (arcp_destroy_f) destroy);

	return 0;

undo4:
	smenvg_destroy(&drum->pitchenvg);
undo3:
	smenvg_destroy(&drum->noiseenvg);
undo2:
	smenvg_destroy(&drum->toneenvg);
undo1:
	smsynth_destroy(&drum->osc);
undo0:
	return r;
}

struct smdrum *smdrum_create() {
	struct smdrum *drum;
	int r;

	drum = amalloc(sizeof(struct smdrum));
	if(drum == NULL) {
		return NULL;
	}

	r = smdrum_init(drum, __smdrum_destroy);
	if(r != 0) {
		afree(drum, sizeof(struct smdrum));
		return NULL;
	}

	return drum;
}


