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

#include <stddef.h>
#include <math.h>

/**
 * Tuning array
 */
struct smkey {
	size_t len; /** The length of the array */
	float tuning[]; /** The array */
};

/**
 * A
 */
#define SMKEY_A 220.0

/**
 * A#
 */
#define SMKEY_A_SHARP 233.081880759045

/**
 * Bb
 */
#define SMKEY_B_FLAT 233.081880759045

/**
 * B
 */
#define SMKEY_B 246.941650628062

/**
 * Cb
 */
#define SMKEY_C_FLAT 246.941650628062

/**
 * C
 */
#define SMKEY_C 261.625565300599

/**
 * B#
 */
#define SMKEY_B_SHARP 261.625565300599

/**
 * C#
 */
#define SMKEY_C_SHARP 277.182630976872

/**
 * Db
 */
#define SMKEY_D_FLAT 277.182630976872

/**
 * D
 */
#define SMKEY_D 293.664767917408

/**
 * D#
 */
#define SMKEY_D_SHARP 311.126983722081

/**
 * Eb
 */
#define SMKEY_E_FLAT 311.126983722081

/**
 * E
 */
#define SMKEY_E 329.62755691287

/**
 * Fb
 */
#define SMKEY_F_FLAT 329.62755691287

/**
 * F
 */
#define SMKEY_F 349.228231433004

/**
 * E#
 */
#define SMKEY_E_SHARP 349.228231433004

/**
 * F#
 */
#define SMKEY_F_SHARP 369.994422711634

/**
 * Gb
 */
#define SMKEY_G_FLAT 369.994422711634

/**
 * G
 */
#define SMKEY_G 391.995435981749

/**
 * G#
 */
#define SMKEY_G_SHARP 415.304697579945

/**
 * Ab
 */
#define SMKEY_A_FLAT 415.304697579945

struct smkey_western {
	size_t length;
	float tuning[7];
};

struct smkey_chromatic {
	size_t length;
	float tuning[12];
};

/**
 * Major tuning
 */
#define SMKEY_MAJOR ((struct smkey *) &smkey_major)

extern struct smkey_western smkey_major;

/**
 * Minor tuning
 */
#define SMKEY_MINOR ((struct smkey *) &smkey_minor)

extern struct smkey_western smkey_minor;

/**
 * Pythagorean tuning
 */
#define SMKEY_PYTHAGOREAN ((struct smkey *) &smkey_pythagorean)

extern struct smkey_chromatic smkey_pythagorean;

/**
 * Equal temperament
 */
#define SMKEY_EQUAL ((struct smkey *) NULL)

/**
 * Transform a note into a frequency according to the semantics of
 * this particular key.
 */
static inline float smkey(struct smkey *key, float root, float note) {
	if (key == SMKEY_EQUAL) {
		return root * powf(2.0f, note/12.0f);
	} else {
		int key_len, n, m, e;
		float freq, f, n_f;

		key_len = key->len;
		f = note;
		n_f = floorf(f);
		f -= n_f;
		n = (int) n_f;
		m = n % key_len;
		e = n / key_len;
		if (m < 0) {
			e--;
			m = key_len + m;
		}
		freq = key->tuning[m];
		if (f != 0) {
			if (m == (key_len - 1)) {
				freq *= powf(2/key->tuning[m], f);
			} else {
				freq *= powf(key->tuning[m + 1]/key->tuning[m],
					     f);
			}
		}
		if (e >= 0) {
			return (freq * root * ((float) (1 << e)));
		} else {
			e = -e;
			return (freq * root / ((float) (1 << e)));
		}
	}
}

#endif /* ! SONICMATHS_KEY_H */
