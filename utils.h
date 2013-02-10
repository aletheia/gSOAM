/**
 *  Copyright 2009 Universita' degli Studi di Pavia
 *  Laboratorio di Visione Artificiale
 *  http://vision.unipv.it
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

float squaredEuclideanDistance(float *v1, float *v2, int dimension);

int rand_int(int min_val, int max_val);
float rand_float(float min_val, float max_val);

void wait(int seconds);
void waitUntilKeyPressed();

#endif