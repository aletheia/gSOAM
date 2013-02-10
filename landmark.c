#include "landmark.h"

float* landmarks;
int firstEmptyLandmark;

// RETURN 0 if error
// RETURN 1 if insertion 

int landmarkInsert(float pos[]) 
{
	int i = 0;
	float nextEmpty = 0;

	if(firstEmptyLandmark == MAX_LANDMARKS) 
	{
		// MEMORIA LANDMARK PIENA
		printf("ERROR INSERT LANDMARK: MEMORY IS FULL!!!\n");
		return 0;
	} else {
		// INSERT NEW LANDMARK
		nextEmpty = landmarks[firstEmptyLandmark*DIMENSION + 1];
		for(i = 0; i < DIMENSION; i++) 
		{
			landmarks[firstEmptyLandmark*DIMENSION + i] = pos[i];
		}
		// CORRECT OFFSETS 
		if(nextEmpty == 0xFFFFFFFF)
			firstEmptyLandmark++;
		else
			firstEmptyLandmark = nextEmpty;
	}

	printf("NEW LANDMARK: %f %f %f | firstEmpty = %d\n", pos[0], pos[1], pos[2], firstEmptyLandmark); 
	return 1;
}

// RETURN 0 if error (index not valid)
// RETURN 1 if remove is successfull

int landmarkRemove(int index) 
{
	int i = 0;
	float empty = 0;

	//check if index is valid ...
	if(index > MAX_LANDMARKS || landmarks[index*DIMENSION + 1] == 0xFFFFFFFF || landmarks[index*DIMENSION] == 0xFFFFFFFF) {
		printf("ERROR REMOVING: INDEX NOT VALID!!!\n");
		return 0;
	}

	if(index < firstEmptyLandmark) 
	{
		// REMOVE AT BEGINNING

		landmarks[index*DIMENSION] = 0xFFFFFFFF;
		landmarks[index*DIMENSION + 1] = firstEmptyLandmark;
		firstEmptyLandmark = index;

	} else if (index > firstEmptyLandmark) { 

		// REMOVE IN THE MIDDLE

		// searching previous empty landmark ...
		for(i = index - 1; i >= 0; i--) 
		{
			if(landmarks[i*DIMENSION] == 0xFFFFFFFF) 
			{
				empty = landmarks[i*DIMENSION + 1];
				landmarks[i*DIMENSION + 1] = index;
				i = 0;
			}
		} 

		landmarks[index*DIMENSION] = 0xFFFFFFFF;
		if(empty == index) 
			landmarks[index*DIMENSION + 1] = 0xFFFFFFFF;
		else
			landmarks[index*DIMENSION + 1] = empty;	
	}

	printf("REM LANDMARK: %d | firstEmpty = %d\n", index, firstEmptyLandmark);
	return 1;
}


// RETURN 0 if memory allocation error
// RETURN 1 if allocation is successfull

int landmarkCreateArray (float** ptr, int* offset) 
{
	int i = 0;
	landmarks = (float*)malloc(MAX_LANDMARKS*DIMENSION*sizeof(float));
	if(landmarks == 0) {
		printf("MEMORY ALLOCATION ERROR!!!\n");	
		return 0;
	}
	
	// Reset memory
	for (i = 0; i < MAX_LANDMARKS*DIMENSION; i++) 
		landmarks[i] = 0xFFFFFFFF;

	*offset = 0;

	return 1;
}

