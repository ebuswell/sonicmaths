/** @file key.h
 *
 * Key
 *
 * Key translates notes into frequencies, scaled by the sample
 * frequency.
 *
 * It does so according to an array which corresponds to the scale:
 *
 * @verbatim
                n/l
r * t[n % l] * 2
@endverbatim
 *
 * where r is the root, t is the tuning array, n is the note, and l is
 * the length of the array.  Note that n / l is truncated integer
 * arithmetic, such that n / l is the integer amount, n % l the
 * remainder.
 *
 * Additionally, notes may be fractional.  Fractional notes correspond
 * to the equation:
 *
 * @verbatim
           f
p * (n / p)
@endverbatim
 *
 * where p is the previous note, n is the next note, and f is the
 * fractional portion.
 *
 * Note that in the case of equal temperament, this all just reduces
 * to:
 *
 * @verbatim
     n
r * 2
@endverbatim
 *
 * Key comes with a number of pre-defined scales.  Minor, Major,
 * Equal-Tempered, and Pythagorean.  If you wish to define your own,
 * pass an array of fractional values between 1 and 2 that correspond
 * to the notes in your scale.  If you have a favorite somewhat
 * conventional scale that you think should be predefined, file a bug
 * or send an email with the fractional coefficients of each note, and
 * I'll probably add it.
 */
/*
 * Copyright 2011 Evan Buswell
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
#ifndef SONICMATHS_KEY_H
#define SONICMATHS_KEY_H 1

#include <stddef.h>
#include <graphline.h>
#include <sonicmaths/graph.h>
#include <sonicmaths/parameter.h>
#include <atomickit/atomic-float.h>
#include <atomickit/spinlock.h>

/**
 * Tuning array
 */
struct smaths_key_tuning {
    size_t tuning_length; /** The length of the array */
    float tuning[1]; /** The array */
};

/**
 * Key
 */
struct smaths_key {
    struct smaths_graph *graph; /** Graph for this key */
    struct gln_node node; /** Node for this key */
    struct smaths_parameter note; /** Input note */
    struct smaths_parameter root; /** Root frequency, as a fraction of sample rate */
    struct gln_socket freq; /** Output frequency, as a fraction of sample rate */
    const struct smaths_key_tuning *tuning; /** The tuning, of type struct smaths_key_tuning */
    spinlock_t tuning_lock; /** For synchronizing changes in the tuning */
    atomic_float_t last_root;
};

/**
 * Initialize key
 */
int smaths_key_init(struct smaths_key *key, struct smaths_graph *graph);

/**
 * Destroy key
 */
void smaths_key_destroy(struct smaths_key *key);

/**
 * Set tuning
 *
 * @param tuning the tuning array.  A copy is made for internal use
 * and the variable passed is not referenced after return, unless
 * using one of the predefined values.
 */
int smaths_key_set_tuning(struct smaths_key *key, const struct smaths_key_tuning *tuning);

/**
 * Transform a note into a frequency according to the semantics of
 * this particular key.
 */
float smaths_key_note2freq(struct smaths_key *key, float note);

/**
 * A
 */
#define SMATHS_A 220.0

/**
 * A#
 */
#define SMATHS_A_SHARP 233.081880759045

/**
 * Bb
 */
#define SMATHS_B_FLAT 233.081880759045

/**
 * B
 */
#define SMATHS_B 246.941650628062

/**
 * Cb
 */
#define SMATHS_C_FLAT 246.941650628062

/**
 * C
 */
#define SMATHS_C 261.625565300599

/**
 * B#
 */
#define SMATHS_B_SHARP 261.625565300599

/**
 * C#
 */
#define SMATHS_C_SHARP 277.182630976872

/**
 * Db
 */
#define SMATHS_D_FLAT 277.182630976872

/**
 * D
 */
#define SMATHS_D 293.664767917408

/**
 * D#
 */
#define SMATHS_D_SHARP 311.126983722081

/**
 * Eb
 */
#define SMATHS_E_FLAT 311.126983722081

/**
 * E
 */
#define SMATHS_E 329.62755691287

/**
 * Fb
 */
#define SMATHS_F_FLAT 329.62755691287

/**
 * F
 */
#define SMATHS_F 349.228231433004

/**
 * E#
 */
#define SMATHS_E_SHARP 349.228231433004

/**
 * F#
 */
#define SMATHS_F_SHARP 369.994422711634

/**
 * Gb
 */
#define SMATHS_G_FLAT 369.994422711634

/**
 * G
 */
#define SMATHS_G 391.995435981749

/**
 * G#
 */
#define SMATHS_G_SHARP 415.304697579945

/**
 * Ab
 */
#define SMATHS_A_FLAT 415.304697579945

/**
 * Major tuning
 */
extern const struct smaths_key_tuning *SMATHS_MAJOR_TUNING;

/**
 * Minor tuning
 */
extern const struct smaths_key_tuning *SMATHS_MINOR_TUNING;

/**
 * Pythagorean tuning
 */
extern const struct smaths_key_tuning *SMATHS_PYTHAGOREAN_TUNING;

/**
 * Equal temperament
 */
#define SMATHS_EQUAL_TUNING ((struct smaths_key_tuning *) -1)

#endif /* ! SONICMATHS_KEY_H */
