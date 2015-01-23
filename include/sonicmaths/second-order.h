/** @file second-order.h
 *
 * Second-order filter
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
#ifndef SONICMATHS_SECOND_ORDER_H
#define SONICMATHS_SECOND_ORDER_H 1

/**
 * Second-order filter
 */
struct sm2order {
	float x1; /** Previous input */
	float x2; /** Previous previous input */
	float y1; /** Previous output */
	float y2; /** Previous previous output */
};

/**
 * Initialize second-order filter
 */
int sm2order_init(struct sm2order *filter);

/**
 * Destroy second-order filter
 */
void sm2order_destroy(struct sm2order *filter);

#endif /* ! SONICMATHS_SECOND_ORDER_H */
