/** @file limiter.h
 *
 * Limiter filter
 *
 * Limits ("distorts") input according to one of the following equations,
 * where s is @c sharpness, g is @c gain, and x is the original input.
 *
 * Lower values of @c sharpness make the sound less "warm" and vice versa.
 *
 * For <tt>Exponential</tt>:
 *
 * @verbatim
         -s(gx - 1)
    log(e           + 1)
1 - --------------------
            -s
       log(e   + 1)
@endverbatim
 *
 * With a symmetrical equation for a negative x.
 *
 * For <tt>Hyperbolic</tt>:
 *
 * @verbatim
      gx
--------------
     s     1/s
(|gx|  + 1)
@endverbatim
 *
 * For <tt>Arctangent</tt>:
 *
 * @verbatim
2atan(sgx)
----------
    π
@endverbatim
 *
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
#ifndef SONICMATHS_LIMITER_H
#define SONICMATHS_LIMITER_H 1

#include <math.h>

enum smlimit_kind {
	SMLIMIT_EXP, SMLIMIT_HYP, SMLIMIT_ATAN
};


void smlimit(enum smlimit_kind kind, int n, float *y, float *x,
	     float *sharpness);

#endif /* ! SONICMATHS_LIMITER_H */
