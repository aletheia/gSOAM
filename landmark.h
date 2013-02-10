#include <stdio.h>
#include <stdlib.h>

#ifndef LANDMARK_H_
#define LANDMARK_H_

	#define MAX_LANDMARKS 100
	#define DIMENSION 3

	// VARS

	extern float* landmarks;
	extern int firstEmptyLandmark;

	// FUNCTIONS
	int landmarkInsert(float pos[]);
	int landmarkRemove(int index);
	int landmarkCreateArray(float** ptr, int* offset);

#endif