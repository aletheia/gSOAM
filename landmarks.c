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


#include <string.h>
#include "landmarks.h"


int landmarks_space_dimension = -1;

// TODO: implement an errno mechanism to signal errors
int landmarks_errno = 0;

/** Gets the space dimension **/
int landmarks_getSpaceDimension() {
	return landmarks_space_dimension;
}

/** Initializes the library by setting the space dimension **/
void landmarks_setSpaceDimension(int dimension) {
#if DEBUG
	if (landmarks_space_dimension != -1)
		fprintf(stderr, "landmarks.c:landmarks_setSpaceDimension: value has been set more than once\n");	
#endif
	
	landmarks_space_dimension = dimension;
}

/** Initializes a data structure for landmarks **/
void landmarks_initialize(float *data, int size, int *freeList) {

	// Mark the first landmark as the free list terminator
	// *((int *) data) = LANDMARKS_MARKER;
	// *((int *) (data + 1)) = LANDMARKS_MARKER;
	memset((void *) data, LANDMARKS_MARKER, size * landmarks_space_dimension * sizeof(float));

	// Set the free list pointer 
	*freeList = 0;

#if DEBUG
	fprintf(stderr, "landmarks.c:landmarks_initialize: memory initialized.\n");
#endif
}


// DEPRECATED
/** Creates a data structure for landmarks **/
float *landmarks_new(int size, int *freeList) {
	float *data;
	
	data = (float *) calloc(size * landmarks_space_dimension, sizeof(float));

	if (data == 0) {
#if DEBUG
		fprintf(stderr, "landmarks.c:landmarks_new: memory allocation error\n");	
#endif
		return data;
	}
#if DEBUG
	fprintf(stderr, "landmarks.c:landmarks_new: new instance allocated.\n");
#endif

	landmarks_initialize(data, size, freeList);

	return data;
}


/** Returns the number of active landmarks **/
int landmarks_getCount(float *data, int size, int freeList) {
	return size - landmarks_getFreeListLength(data, size, freeList);
}


/** Returns the number of free landmarks **/
int landmarks_getFreeListLength(float *data, int size, int freeList) {
	int count;
	int current, next;

	if (freeList == LANDMARKS_MARKER)
		return 0;

	count = 0;
	current = freeList;
	next = *((int *) (data + current * landmarks_space_dimension + 1));
	while (next != LANDMARKS_MARKER && next < size) {
		current = next;
#if DEBUG
		if (*((int *) (data + current * landmarks_space_dimension)) != LANDMARKS_MARKER) {
			fprintf(stderr, "landmarks.c:landmarks_getFreeListLength: invalid landmark in free list %d\n", current); 
		}
#endif
		next = *((int *) (data + current * landmarks_space_dimension + 1));
		count++;
	}
	if (next == size)
		// Data beyond the list terminator is not empty
		return count + 1;
	else
		return count + (size - current);
}

// DEPRECATED
/** Inserts a new landmark and updates the freeList - CPU usage only **/
int landmarks_newLandmark(float *landmark, float *data, int size, int *freeList)
{
	int index;
	int i;

	index = landmarks_newLandmarkIndex(data, size, freeList);

	if (index < 0)
		return index;

	// Copy landmark in place
	for(i = 0; i < landmarks_space_dimension; i++) 
		data[index * landmarks_space_dimension + i] = landmark[i];

#if DEBUG
	fprintf(stderr, "landmarks.c:landmarks_newLandmark: new landmark inserted %d\n", index); 
#endif
	return index;
}

/** Inserts a new landmark and updates the freeList **/
int landmarks_newLandmarkIndex(float *data, int size, int *freeList) 
{
	int current, next;

	if (*freeList == LANDMARKS_MARKER) 
	{
#if DEBUG
		fprintf(stderr, "landmarks.c:landmarks_newLandmarkIndex: out of data memory\n");
#endif
		return -1;
	} 

	current = *freeList;
	next = *((int *) (data + current * landmarks_space_dimension + 1));

	if (next == LANDMARKS_MARKER) 
	{
		if (*freeList < size - 1) 
		{
			// Advance list terminator
			*freeList += 1;
			*((int *) (data + (*freeList) * landmarks_space_dimension)) = LANDMARKS_MARKER;
			*((int *) (data + (*freeList) * landmarks_space_dimension + 1)) = LANDMARKS_MARKER;
		}
		else 
		{
			// data structure is full
			*freeList = LANDMARKS_MARKER;
		}
	}
	else
		*freeList = next;

#if DEBUG
	fprintf(stderr, "landmarks.c:landmarks_newLandmarkIndex: new landmark index created %d\n", current); 
#endif
	return current;
}

/** Frees a landmark index and inserts it into the free list **/
void landmarks_freeLandmarkIndex(int index, float *data, int size, int *freeList)
{
	int current, next;

	next = *freeList;
	if (*freeList == LANDMARKS_MARKER)
	{
		// It was at limit capacity: use end of data as new list terminator
		*((int *) (data + index * landmarks_space_dimension)) = LANDMARKS_MARKER;
		*((int *) (data + index * landmarks_space_dimension + 1)) = size;
		*freeList = index;
	}
	else if (index < next) 
	{
		// Insert it at the beginning of the free list
		*((int *) (data + index * landmarks_space_dimension)) = LANDMARKS_MARKER;
		*((int *) (data + index * landmarks_space_dimension + 1)) = next;
		*freeList = index;
	} 
	else 
	{
		// Search the free list
		while (index > next && next < size) {
			current = next;
#if DEBUG
			if (*((int *) (data + current * landmarks_space_dimension)) != LANDMARKS_MARKER ||
				next == LANDMARKS_MARKER)
			{
				fprintf(stderr, "landmarks.c:landmarks_freeLandmarkIndex: invalid landmark in free list %d\n", current); 
			}
#endif
			next = *((int *) (data + current * landmarks_space_dimension + 1));
		}
		// Insert it between current and next
		*((int *) (data + current * landmarks_space_dimension + 1)) = index;
		*((int *) (data + index * landmarks_space_dimension)) = LANDMARKS_MARKER;
		*((int *) (data + index * landmarks_space_dimension + 1)) = next;
	}

#if DEBUG
	fprintf(stderr, "landmarks.c:landmarks_freeLandmarkIndex: landmark freed %d\n", index); 
#endif
}

/** Returns 1 if the landmark at a given index is not marked as free **/
int landmarks_isActive(int index, float *data, int size)
{
#if DEBUG
	if (index < 0 || index >= size)
	{
		fprintf(stderr, "landmarks.c:landmarks_isActive: invalid index %d\n", index); 
	}
#endif
	return *((int *) (data + index * landmarks_space_dimension)) != LANDMARKS_MARKER;
}

/** Returns 1 if the landmark at a given index is marked as free and is the data terminator **/
int landmarks_isEndOfData(int index, float *data, int size)
{
#if DEBUG
	if (index < 0 || index >= size)
	{
		fprintf(stderr, "landmarks.c:landmarks_isEndOfLandmarks: invalid index %d\n", index); 
	}
#endif
	return *((int *) (data + index * landmarks_space_dimension)) == LANDMARKS_MARKER && 
		*((int *) (data + index * landmarks_space_dimension + 1)) == LANDMARKS_MARKER;
}


#ifndef landmarks_isActiveValue

/** Returns 1 if the value pointed at is not marked **/
int landmarks_isActiveValue(float *value)
{
	return *((int *) value) != LANDMARKS_MARKER;
}

#endif 

