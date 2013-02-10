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


#include "landmarks.h"

#include "utils.h"


int test_landmarks_creation()
{
	float *landmarks;
	int size = 750;
	int freeList;

	int dimension;

	int result;

	dimension = landmarks_getSpaceDimension();
	result = (dimension > 0);

	landmarks = landmarks_new(size, &freeList);
    result = result && (landmarks != 0) && (freeList == 0);	

	fprintf(stderr, "Landmarks_test.c:test_landmarks_creation: data structure creation... %s\n", result ? "OK" : "FAILED");

	free(landmarks);

	// Return 0 it the test was OK
	return result ? 0 : 1;
}

int test_landmarks_basicInsertRemove()
{
	float *landmarks;
	int size = 750;
	int freeList;

	int dimension;
	float *landmark;
	int count;
	int freeCount;
	int index;

	int result;

	dimension = landmarks_getSpaceDimension();
	result = (dimension > 0);

	landmarks = landmarks_new(size, &freeList);
    result = result && (landmarks != 0) && (freeList == 0);	

	fprintf(stderr, "Landmarks_test.c:test_landmarks_basicInsertRemove: data structure creation... %s\n", result ? "OK" : "FAILED");

	count = landmarks_getCount(landmarks, size, freeList);
	freeCount = landmarks_getFreeListLength(landmarks, size, freeList);
	result = result && (count == 0) && (freeCount == size);

	index = 0;
	count = !landmarks_isActive(index, landmarks, size);
	result = result && count;

	fprintf(stderr, "Landmarks_test.c:test_landmarks_basicInsertRemove: active landmarks and free list length... %s\n", result ? "OK" : "FAILED");

	landmark = (float*) calloc(dimension, sizeof(float));
	landmark[0] = 10.5f;
	landmark[1] = 4.5f;
	landmark[2] = 3.0f;

	index = landmarks_newLandmark(landmark, landmarks, size, &freeList);
	result = result && (index == 0); 

	count = landmarks_getCount(landmarks, size, freeList);
	freeCount = landmarks_getFreeListLength(landmarks, size, freeList);
	result = result && (count == 1) && (freeCount == size - 1);

	count = landmarks_isActive(index, landmarks, size);
	result = result && count;

	result = result && (landmark[0] == landmarks[0]) && (landmark[1] == landmarks[1]) && (landmark[2] == landmarks[2]);

	fprintf(stderr, "Landmarks_test.c:test_landmarks_basicInsertRemove: new landmark insertion... %s\n", result ? "OK" : "FAILED");

	free(landmark);
	free(landmarks);

	// Return 0 it the test was OK
	return result ? 0 : 1;
}


int test_landmarks_sparseInsertRemove()
{
	float *landmarks;
	int size = 750;
	int freeList;

	int dimension;
	int count;
	int freeCount;
	int index;
	int i;

	int result;

	// All memory allocations and loops must be explicit

	dimension = landmarks_getSpaceDimension();
	result = (dimension > 0);

	landmarks = (float *) calloc(size * dimension, sizeof(float));
	landmarks_initialize(landmarks, size, &freeList);
    result = result && (landmarks != 0) && (freeList == 0);	

	fprintf(stderr, "Landmarks_test.c:test_landmarks_sparseInsertRemove: data structure creation... %s\n", result ? "OK" : "FAILED");

	count = landmarks_getCount(landmarks, size, freeList);
	freeCount = landmarks_getFreeListLength(landmarks, size, freeList);
	result = result && (count == 0) && (freeCount == size);

	index = 0;
	count = !landmarks_isActive(index, landmarks, size);
	result = result && count;

	fprintf(stderr, "Landmarks_test.c:test_landmarks_sparseInsertRemove: active landmarks and free list length... %s\n", result ? "OK" : "FAILED");

	index = landmarks_newLandmarkIndex(landmarks, size, &freeList);
	result = result && (index == 0) && (freeList = 1); 
	
	landmarks[index * dimension] = 1.f;
	landmarks[index * dimension + 1] = 1024.5f;
	landmarks[index * dimension + 2] = 2048.5f;

	index = landmarks_newLandmarkIndex(landmarks, size, &freeList);
	result = result && (index == 1) && (freeList = 2); 
	
	landmarks[index * dimension] = 2.f;
	landmarks[index * dimension + 1] = 1024.5f;
	landmarks[index * dimension + 2] = 2048.5f;

	index = landmarks_newLandmarkIndex(landmarks, size, &freeList);
	result = result && (index == 2) && (freeList = 3); 
	
	landmarks[index * dimension] = 3.f;
	landmarks[index * dimension + 1] = 1024.5f;
	landmarks[index * dimension + 2] = 2048.5f;

	index = landmarks_newLandmarkIndex(landmarks, size, &freeList);
	result = result && (index == 3) && (freeList = 4); 
	
	landmarks[index * dimension] = 4.f;
	landmarks[index * dimension + 1] = 1024.5f;
	landmarks[index * dimension + 2] = 2048.5f;

	index = landmarks_newLandmarkIndex(landmarks, size, &freeList);
	result = result && (index == 4) && (freeList = 5); 
	
	landmarks[index * dimension] = 5.f;
	landmarks[index * dimension + 1] = 1024.5f;
	landmarks[index * dimension + 2] = 2048.5f;

	count = landmarks_getCount(landmarks, size, freeList);
	freeCount = landmarks_getFreeListLength(landmarks, size, freeList);
	result = result && (count == 5) && (freeCount == size - 5);

	index = 0;
	count = landmarks_isActive(index, landmarks, size);
	result = result && count && (landmarks[index * dimension] == (float) (index + 1));

	index = 1;
	count = landmarks_isActive(index, landmarks, size);
	result = result && count && (landmarks[index * dimension] == (float) (index + 1));

	index = 2;
	count = landmarks_isActive(index, landmarks, size);
	result = result && count && (landmarks[index * dimension] == (float) (index + 1));

	index = 3;
	count = landmarks_isActive(index, landmarks, size);
	result = result && count && (landmarks[index * dimension] == (float) (index + 1));

	index = 4;
	count = landmarks_isActive(index, landmarks, size);
	result = result && count && (landmarks[index * dimension] == (float) (index + 1));

	result = result && (freeList == 5);

	fprintf(stderr, "Landmarks_test.c:test_landmarks_sparseInsertRemove: 5 landmark insertions... %s\n", result ? "OK" : "FAILED");

	index = 1;
	landmarks_freeLandmarkIndex(index, landmarks, size, &freeList);
	count = landmarks_getCount(landmarks, size, freeList);
	result = result && (freeList == 1) && (count == 4);

	index = 3;
	landmarks_freeLandmarkIndex(index, landmarks, size, &freeList); 
	count = landmarks_getCount(landmarks, size, freeList);
	result = result && (freeList == 1) && (count == 3);

	index = landmarks_newLandmarkIndex(landmarks, size, &freeList);
	result = result && (index == 1) && (freeList == 3); 
	
	index = landmarks_newLandmarkIndex(landmarks, size, &freeList);
	result = result && (index == 3) && (freeList == 5); 

	count = landmarks_getCount(landmarks, size, freeList);
	result = result && (count == 5);

	fprintf(stderr, "Landmarks_test.c:test_landmarks_sparseInsertRemove: 2 sparse removals and insertions... %s\n", result ? "OK" : "FAILED");

	index = 1;
	landmarks_freeLandmarkIndex(index, landmarks, size, &freeList);
	count = landmarks_getCount(landmarks, size, freeList);
	result = result && (freeList == 1) && (count == 4);

	index = 3;
	landmarks_freeLandmarkIndex(index, landmarks, size, &freeList); 
	count = landmarks_getCount(landmarks, size, freeList);
	result = result && (freeList == 1) && (count == 3);

	// Loop over the entire structure
	count = 0;
	for (i = 0; i < size; i++)
	{
		if (landmarks_isActive(i, landmarks, size))
		{
			count++;
			result = result && (landmarks[i * dimension] == (float) (i + 1));
		} 
		else if (landmarks_isEndOfData(i, landmarks, size))
			break;
	}

	result = result && (i == 5) && (count == 3) && (count == landmarks_getCount(landmarks, size, freeList));

	fprintf(stderr, "Landmarks_test.c:test_landmarks_sparseInsertRemove: loop over compact landmarks... %s\n", result ? "OK" : "FAILED");


	free(landmarks);

	// Return 0 it the test was OK
	return result ? 0 : 1;
}

int test_landmarks_stressInsertRemove()
{
	float *landmarks;
	int size = 750;
	int freeList;

	int dimension;
	int count;
	int index;
	int i;

	int result;

	// All memory allocations and loops must be explicit

	dimension = landmarks_getSpaceDimension();
	result = (dimension > 0);

	landmarks = (float *) calloc(size * dimension, sizeof(float));
	landmarks_initialize(landmarks, size, &freeList);
    result = result && (landmarks != 0) && (freeList == 0);	

	fprintf(stderr, "Landmarks_test.c:test_landmarks_stressInsertRemove: data structure creation... %s\n", result ? "OK" : "FAILED");

	for (i = 0; i < 100; i++)
	{
		index = landmarks_newLandmarkIndex(landmarks, size, &freeList);
		result = result && (index == i);
		if (i < size - 1)
			result = result && (freeList == i + 1);
		else
			result = result && (freeList == -1);

		landmarks[index * dimension] = (float) (i + 1);
		landmarks[index * dimension + 1] = 1024.5f;
		landmarks[index * dimension + 2] = 2048.5f;
	}

	count = landmarks_getCount(landmarks, size, freeList);
	result = result && (count == 100);

	fprintf(stderr, "Landmarks_test.c:test_landmarks_stressInsertRemove: inserting 100 landmarks... %s\n", result ? "OK" : "FAILED");

	// Loop over the entire structure
	count = 0;
	for (i = 0; i < size; i++)
	{
		if (landmarks_isActive(i, landmarks, size))
		{
			count++;
			result = result && (landmarks[i * dimension] == (float) (i + 1));
		} 
		else if (landmarks_isEndOfData(i, landmarks, size))
			break;
	}

	result = result && (i == 100) && (count == 100) && (count == landmarks_getCount(landmarks, size, freeList));

	fprintf(stderr, "Landmarks_test.c:test_landmarks_stressInsertRemove: loop over compact landmarks... %s\n", result ? "OK" : "FAILED");

	for (i = 100; i < size; i++)
	{
		index = landmarks_newLandmarkIndex(landmarks, size, &freeList);
		result = result && (index == i);
		if (i < size - 1)
			result = result && (freeList == i + 1);
		else
			result = result && (freeList == -1);

		landmarks[index * dimension] = (float) (i + 1);
		landmarks[index * dimension + 1] = 1024.5f;
		landmarks[index * dimension + 2] = 2048.5f;
	}

	count = landmarks_getCount(landmarks, size, freeList);
	result = result && (count == size);

	fprintf(stderr, "Landmarks_test.c:test_landmarks_stressInsertRemove: data structure filling... %s\n", result ? "OK" : "FAILED");

	index = landmarks_newLandmarkIndex(landmarks, size, &freeList);
	result = result && (index == -1);

	fprintf(stderr, "Landmarks_test.c:test_landmarks_stressInsertRemove: data structure out of memory... %s\n", result ? "OK" : "FAILED");

	index = size - 3;
	landmarks_freeLandmarkIndex(index, landmarks, size, &freeList);
	count = landmarks_getCount(landmarks, size, freeList);
	result = result && (freeList == size - 3) && (count == size - 1);

	count = !landmarks_isActive(index, landmarks, size);
	result = result && count;

	index = size - 1;
	landmarks_freeLandmarkIndex(index, landmarks, size, &freeList);
	count = landmarks_getCount(landmarks, size, freeList);
	result = result && (freeList == size - 3) && (count == size - 2);

	index = size - 2;
	landmarks_freeLandmarkIndex(index, landmarks, size, &freeList);
	count = landmarks_getCount(landmarks, size, freeList);
	result = result && (freeList == size - 3) && (count == size - 3);

	// (size - 1) must be the index of the new free list terminator
	index = size - 1;
	result = result && (*((int *) landmarks + index * dimension + 1) == size);

	fprintf(stderr, "Landmarks_test.c:test_landmarks_stressInsertRemove: three removals at end... %s\n", result ? "OK" : "FAILED");

	count = landmarks_getCount(landmarks, size, freeList);
	for (i = size - 6; i > 0; i -= 3)
	{
		landmarks_freeLandmarkIndex(i, landmarks, size, &freeList);
		--count;
		result = result && (freeList == i);
	}

	result = result && (count == landmarks_getCount(landmarks, size, freeList));

	fprintf(stderr, "Landmarks_test.c:test_landmarks_stressInsertRemove: sparse removals... %s\n", result ? "OK" : "FAILED");

	// Loop over the entire structure
	count = 0;
	for (i = 0; i < size; i++)
	{
		if (landmarks_isActive(i, landmarks, size))
		{
			count++;
			result = result && (landmarks[i * dimension] == (float) (i + 1));
		} 
		else if (landmarks_isEndOfData(i, landmarks, size))
			break;
	}

	result = result && (count == landmarks_getCount(landmarks, size, freeList));

	fprintf(stderr, "Landmarks_test.c:test_landmarks_stressInsertRemove: loop over sparse landmarks... %s\n", result ? "OK" : "FAILED");

	free(landmarks);

	// Return 0 it the test was OK
	return result ? 0 : 1;
}


int main(){
	/** 
		this is a control variable: if one of our test is going to fail, the outcoming value of testSuiteRes will be greater than 0,
		by this way just a simple check on this variable can give informations about our unit tests,
	**/
	int testSuiteRes = 0;

	// Do this once and for all
	landmarks_setSpaceDimension(3);

	/** Fire every test available and sum its result to control variable value. Each test returns 0 if anything is ok and 1 if an error has happenend **/
	testSuiteRes += test_landmarks_creation();

	testSuiteRes += test_landmarks_basicInsertRemove();

	testSuiteRes += test_landmarks_sparseInsertRemove();

	testSuiteRes += test_landmarks_stressInsertRemove();

	if (testSuiteRes != 0)
		fprintf(stderr, "Warning some tests have failed!!\n");
	else 
		fprintf(stderr, "Everything works fine. All tests passed.\n");

	wait(20);
}