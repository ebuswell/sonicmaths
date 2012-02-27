/*
 * util.h
 * 
 * Copyright 2010 Evan Buswell
 * 
 * This file is part of Sonic Maths.
 * 
 * Sonic Maths is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 2 of the License,
 * or (at your option) any later version.
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

/* Should this file maybe be absorbed into sonicmaths/math.h? */

#ifndef UTIL_H
#define UTIL_H 1

#include <math.h>

/* e^(-pi) */
#define M_E_PI 0.0432139182637722497745319681207082711488387893098925621783723732

/* scale x to an exponential scale suitable for attenuation */
#define L2ESCALE(x) ((expf(-M_PI*(1.0f - (x))) - M_E_PI)/(1.0f - M_E_PI))

/* scale x to a linear scale, where x is scaled according to a frequency scale */
#define F2LSCALE(x) (log2((x) + 1.0f))

/* scale x to an attenuation exponential scale, where x is scaled according to a frequency exponential scale */
#define F2ESCALE(x) L2ESCALE(F2LSCALE(x))

#endif /* ! UTIL_H */
