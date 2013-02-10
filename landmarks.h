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


#ifndef LANDMARKS_H_
#define LANDMARKS_H_

#include <stdio.h>
#include <stdlib.h>

#include "commons.h"


int landmarks_getSpaceDimension();
void landmarks_setSpaceDimension(int dimension);

// DEPRECATED
float *landmarks_new(int size, int *freeList);
// DEPRECATED
int landmarks_newLandmark(float *landmark, float *data, int size, int *freeList);

void landmarks_initialize(float *data, int size, int *freeList);

int landmarks_getCount(float *data, int size, int freeList);
int landmarks_getFreeListLength(float *data, int size, int freeList);

int landmarks_newLandmarkIndex(float *data, int size, int *freeList);
void landmarks_freeLandmarkIndex(int index, float *data, int size, int *freeList);

int landmarks_isActive(int index, float *data, int size);
int landmarks_isEndOfData(int index, float *data, int size);

/** VARNING: This works only if the size of both types int and float is 32 **/
#define LANDMARKS_MARKER 0xFFFFFFFF

#define landmarks_isActiveCell(ptr) (*((int *) ptr) != LANDMARKS_MARKER)
#define landmarks_markInactiveCell(ptr) (*((int *) ptr) = LANDMARKS_MARKER)

#endif