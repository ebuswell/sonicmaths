/** @file predefs.h
 * Predefs
 *
 * Predefined full synth modules.
 */
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
#ifndef SONICMATHS_PREDEFS_H
#define SONICMATHS_PREDEFS_H 1

#include <math.h>
#include <atomickit/rcp.h>
#include <sonicmaths/synth.h>
#include <sonicmaths/sine.h>
#include <sonicmaths/impulse-train.h>
#include <sonicmaths/integrator.h>
#include <sonicmaths/second-order.h>
#include <sonicmaths/envelope-generator.h>
#include <sonicmaths/bandpass.h>
#include <sonicmaths/lowpass.h>
#include <sonicmaths/noise.h>
#include <sonicmaths/distortion.h>
#include <sonicmaths/math.h>

#define SMASYNTH_NSYNTHS 4
#define SMASYNTH_NINTGS 8

struct smasynth {
	struct arcp_region;
	struct smsynth synth[4];
	struct smintg intg[8];
	struct smenvg aenvg;
	struct smenvg fenvg;
	struct sm2order filterlp;
	struct sm2order filterres;
	struct smsynth lfo;
};

enum smasynth_type {
	SMNONE = 0x0,
	SMSINE = 0x8,
	SMSAW = 0x1,
	SMBOLA = 0x2,
	SMSQUARE = 0x5,
	SMTRIANGLE = 0x6,
	SMINTERFERENCE = 0x4
};

void smasynth_destroy(struct smasynth *asynth);

int smasynth_init(struct smasynth *asynth,
                  void (*destroy)(struct smasynth *));

struct smasynth *smasynth_create(void);

static inline int smasynth_redim(struct smasynth *asynth, int nchannels) {
	int i, r;

	for(i = 0; i < SMASYNTH_NSYNTHS; i++) {
 	       r = smsynth_redim(&asynth->synth[i], nchannels);
	       if(r != 0) {
		       return r;
	       }
	}

	for(i = 0; i < SMASYNTH_NINTGS; i++) {
		r = smintg_redim(&asynth->intg[i], nchannels);
		if(r != 0) {
			return r;
		}
	}

	r = smenvg_redim(&asynth->aenvg, nchannels);
	if(r != 0) {
		return r;
	}

	r = smenvg_redim(&asynth->fenvg, nchannels);
	if(r != 0) {
		return r;
	}

	r = sm2order_redim(&asynth->filterlp, nchannels);
	if(r != 0) {
		return r;
	}

	r = sm2order_redim(&asynth->filterres, nchannels);
	if(r != 0) {
		return r;
	}

	r = smsynth_redim(&asynth->lfo, nchannels);
	if(r != 0) {
		return r;
	}

	return 0;
}

static inline float smasynth_osc(struct smasynth *asynth, int channel, int num,
                                 float f, enum smasynth_type osc, float interphase) {
	float y1;
	float y2;

	if(osc == SMNONE) {
		return 0;
	} else if(osc == SMSINE) {
		return smsine(&asynth->synth[2 * num], channel, f, 0.0f);
	} else {
		y1 = smitrain(&asynth->synth[2 * num], channel, f, 0.0f);
		y1 = f * smintg(&asynth->intg[4 * num], channel, y1);
		if(osc & SMBOLA) {
			y1 = f * smintg(&asynth->intg[4 * num + 1], channel, y1);
		}
		if(osc & SMINTERFERENCE) {
			y2 = smitrain(&asynth->synth[2 * num + 1], channel, f, interphase);
			y2 = f * smintg(&asynth->intg[4 * num + 2], channel, y2);
			if(osc & SMBOLA) {
				y2 = f * smintg(&asynth->intg[4 * num + 3], channel, y2);
			}
			return y1 + y2;
		} else {
			return y1;
		}
	}
}

struct smasynth_param {
	enum smasynth_type osc1;
	float interphase1;
	float amp1;
	float detune1;
	enum smasynth_type osc2;
	float interphase2;
	float amp2;
	float detune2;
	float attack;
	float decay;
	float sustain;
	float release;
	float filtermin;
	float filtermax;
	float resonance;
	float lforate;
	float lfopitch;
	float lfofilter;
};

static inline float smasynth(struct smasynth *asynth,
                             int channel, float f, float ctl,
			     struct smasynth_param *p) {
	float y, o, lfo, ff, fmin, fmax, fsus;

	lfo = smsine(&asynth->lfo, channel, p->lforate, 0.0f);

	o = p->amp1 * smasynth_osc(asynth, channel, 0,
	                           f * powf(2, (p->detune1 + p->lfopitch * lfo)/12),
	                           p->osc1, p->interphase1);
	o += p->amp2 * smasynth_osc(asynth, channel, 1,
	                            f * powf(2, (p->detune2 + p->lfopitch * lfo)/12),
	                            p->osc2, p->interphase2);

	fmin = f * powf(2, p->filtermin);
	fsus = f * powf(2, p->filtermin + p->sustain * (p->filtermax - p->filtermin));
	fmax = f * powf(2, p->filtermax);

	ff = smenvg(&asynth->fenvg, channel, false, ctl,
	            p->attack, fmax, p->decay, fsus, p->release, fmin);

	ff *= powf(2, p->lfofilter * lfo);

	y = smlowpass(&asynth->filterlp, channel, o, ff, 1);
	y += p->resonance
	     * smbandpass(&asynth->filterres, channel, o, ff, 1);
	y *= smenvg(&asynth->aenvg, channel, false, ctl,
	            p->attack, 1, p->decay, p->sustain, p->release, 0);

	return y;
}

struct smdrum {
	struct arcp_region;
	struct smsynth osc;
	struct smenvg toneenvg;
	struct smenvg noiseenvg;
	struct smenvg pitchenvg;
	struct sm2order filterlp;
};

void smdrum_destroy(struct smdrum *drum);

int smdrum_init(struct smdrum *drum,
                  void (*destroy)(struct smdrum *));

struct smdrum *smdrum_create(void);

static inline int smdrum_redim(struct smdrum *drum, int nchannels) {
	int r;

	r = smsynth_redim(&drum->osc, nchannels);
	if(r != 0) {
		return r;
	}

	r = smenvg_redim(&drum->toneenvg, nchannels);
	if(r != 0) {
		return r;
	}

	r = smenvg_redim(&drum->noiseenvg, nchannels);
	if(r != 0) {
		return r;
	}

	r = smenvg_redim(&drum->pitchenvg, nchannels);
	if(r != 0) {
		return r;
	}

	r = sm2order_redim(&drum->filterlp, nchannels);
	if(r != 0) {
		return r;
	}

	return 0;
}

static inline float smdrum(struct smdrum *drum,
                           int channel, float f, float ctl,
                           float attack, float decay, float toneamp,
                           float noiseattack, float noisedecay, float noiseamp,
                           float pitchdrop, float punch,
                           float cutoff) {
	float y, o, n, fenv, oenv, nenv, pitchmin;

	pitchmin = f * powf(2, -pitchdrop/12);

	fenv = smenvg(&drum->pitchenvg, channel, false, ctl,
	              attack, f, decay, pitchmin, decay, pitchmin);
	fenv = f * smdistort((1 + punch) * fenv / f, SMDISTORT_EXP, 2);

	o = smsine(&drum->osc, channel, fenv, 0.0f);
	n = smnoise(SMNOISE_GAUSSIAN);
	oenv = smenvg(&drum->toneenvg, channel, false, ctl,
	              attack, 1, decay, 0, decay, 0);
	oenv = smdistort((1 + punch) * oenv, SMDISTORT_EXP, 2);
	o *= oenv;
	nenv = smenvg(&drum->noiseenvg, channel, false, ctl,
	              noiseattack, 1, noisedecay, 0, noisedecay, 0);
	nenv = smdistort((1 + punch) * nenv, SMDISTORT_EXP, 2);
	n *= nenv;
	y = o * toneamp + n * noiseamp;
	y = smlowpass(&drum->filterlp, channel, y, cutoff, 1);

	return y;
}

static inline float smkick(struct smdrum *drum, int channel, float ctl, float sample_rate,
                           float decay) {
	return smdrum(drum, channel, smaths_normfreq(sample_rate, 58), ctl,
	              smaths_normtime(sample_rate, 0.02), decay, 1,
		      smaths_normtime(sample_rate, 0.001), decay, 0.55,
	              -7, 1.5, smaths_normfreq(sample_rate, 165));
}

static inline float smsnare(struct smdrum *drum, int channel, float ctl, float sample_rate,
                            float decay) {
	return smdrum(drum, channel, smaths_normfreq(sample_rate, 220), ctl,
	              smaths_normtime(sample_rate, 0.02), decay, 0.08,
		      smaths_normtime(sample_rate, 0.02), decay, 0.7,
	              -7, 1.5, smaths_normfreq(sample_rate, 10000));
}

#endif
