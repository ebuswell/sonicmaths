/** @file key.h
 *
 * Key
 *
 * Key translates notes into frequencies, scaled by the sample frequency.
 *
 * It does so according to an array which corresponds to the scale:
 *
 * @verbatim
                n/l
r * t[n % l] * 2
@endverbatim
 *
 * where r is the root, t is the tuning array, n is the note, and l is the
 * length of the array.  Note that n / l is truncated integer arithmetic, such
 * that n / l is the integer amount, n % l the remainder.
 *
 * Additionally, notes may be fractional.  Fractional notes correspond to the
 * equation:
 *
 * @verbatim
           f
p * (n / p)
@endverbatim
 *
 * where p is the previous note, n is the next note, and f is the fractional
 * portion.
 *
 * Note that in the case of equal temperament, this all just reduces to:
 *
 * @verbatim
     n
r * 2
@endverbatim
 *
 * Key comes with a number of pre-defined scales.  Minor, Major,
 * Equal-Tempered, and Pythagorean.  If you wish to define your own, pass an
 * array of fractional values between 1 and 2 that correspond to the notes in
 * your scale.  If you have a favorite somewhat conventional scale that you
 * think should be predefined, file a bug or send an email with the fractional
 * coefficients of each note, and I'll probably add it.
 */
/*
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
#ifndef SONICMATHS_KEY_H
#define SONICMATHS_KEY_H 1

#include <math.h>

static inline float smn2fv(float note, float root) {
	return root * powf(2.0f, note);
}

static inline float smf2nv(float freq, float root) {
	return log2f(freq / root);
}

void smn2f(int n, float *freq, float *note, float *root);

void smf2n(int n, float *note, float *freq, float *root);

/**
 * Tuning array
 */
struct smkey {
	int len; /** The length of the array */
	float tuning[]; /** The array */
};

/**
 * A
 */
#define SMKEYF_A 220.0

/**
 * A#
 */
#define SMKEYF_A_SHARP 233.081880759045

/**
 * Bb
 */
#define SMKEYF_B_FLAT 233.081880759045

/**
 * B
 */
#define SMKEYF_B 246.941650628062

/**
 * Cb
 */
#define SMKEYF_C_FLAT 246.941650628062

/**
 * C
 */
#define SMKEYF_C 261.625565300599

/**
 * B#
 */
#define SMKEYF_B_SHARP 261.625565300599

/**
 * C#
 */
#define SMKEYF_C_SHARP 277.182630976872

/**
 * Db
 */
#define SMKEYF_D_FLAT 277.182630976872

/**
 * D
 */
#define SMKEYF_D 293.664767917408

/**
 * D#
 */
#define SMKEYF_D_SHARP 311.126983722081

/**
 * Eb
 */
#define SMKEYF_E_FLAT 311.126983722081

/**
 * E
 */
#define SMKEYF_E 329.62755691287

/**
 * Fb
 */
#define SMKEYF_F_FLAT 329.62755691287

/**
 * F
 */
#define SMKEYF_F 349.228231433004

/**
 * E#
 */
#define SMKEYF_E_SHARP 349.228231433004

/**
 * F#
 */
#define SMKEYF_F_SHARP 369.994422711634

/**
 * Gb
 */
#define SMKEYF_G_FLAT 369.994422711634

/**
 * G
 */
#define SMKEYF_G 391.995435981749

/**
 * G#
 */
#define SMKEYF_G_SHARP 415.304697579945

/**
 * Ab
 */
#define SMKEYF_A_FLAT 415.304697579945

struct smkey_western {
	int len;
	float tuning[13];
};

/**
 * Harmonically just tuning
 */
#define SMKEY_HARMONIC ((struct smkey *) &smkey_harmonic)

extern struct smkey_western smkey_harmonic;

/**
 * Pythagorean tuning
 */
#define SMKEY_PYTHAGOREAN ((struct smkey *) &smkey_pythagorean)

extern struct smkey_western smkey_pythagorean;

/**
 * Equal temperament tuning
 */
#define SMKEY_EQUAL ((struct smkey *) &smkey_equal)

extern struct smkey_western smkey_equal;

/**
 * Transform a note into a frequency according to the semantics of
 * this particular key.
 */
void smkey(struct smkey *key, int n, float *freq, float *note, float *root);

#endif /* ! SONICMATHS_KEY_H */
