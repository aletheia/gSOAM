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


#include <float.h>

#include "findWinners.h"


#define DIMENSION 4
#define MAX_LANDMARKS (512 * 128)
#define LOAD_LANDMARKS (512 * 128)

#define SAMPLES (512 * 128)
#define F_BOUNDING_BOX_RADIUS 128.0f
#define MAX_DISTANCE FLT_MAX


float *loadSamples()
{
	int i;
	float *samples;
	samples = (float*) malloc(SAMPLES * DIMENSION * sizeof(float));
 
	for(i = 0; i < SAMPLES * DIMENSION; i++){
		samples[i] = rand_float(-F_BOUNDING_BOX_RADIUS, F_BOUNDING_BOX_RADIUS);
	}

	return samples;
}


int main()
{
	float *landmarks, *landmark;
	int freeList;
	
	int firstWinnerIndex;
	int secondWinnerIndex;
	float firstWinnerDistance;
	float secondWinnerDistance;

	int index;

	float *samples;
	float *sample;

	float distance;
	int i, j;

#if TIMING
	clock_t t1;
	clock_t t2;
#endif

	srand((unsigned int) time(NULL));

#if DEBUG
	fprintf(stderr, "findWinners.c: warming Up for execution...\n");
#endif
	/** Warming up **/
	samples = loadSamples();

	landmarks_setSpaceDimension(DIMENSION);

	landmarks = (float *) malloc(MAX_LANDMARKS * DIMENSION * sizeof(float));
	landmarks_initialize(landmarks, MAX_LANDMARKS, &freeList);

#if DEBUG
	fprintf(stderr, "findWinners.c: system ready, variables initialized.\n");
#endif

	/** Insert a Landmark at the position of the first casual sample, in order to have at least one iteration **/
#ifdef LOAD_LANDMARKS
#if DEBUG
	fprintf(stderr, "findWinners.c: initializing list with %d samples...\n", LOAD_LANDMARKS);
#endif

	for (i = 0; i < LOAD_LANDMARKS; i++)
	{
		index = landmarks_newLandmarkIndex(landmarks, MAX_LANDMARKS, &freeList);
		landmark = landmarks + index * DIMENSION;

		for(j = 0; j < DIMENSION; j++)
			landmark[j] = rand_float(-F_BOUNDING_BOX_RADIUS, F_BOUNDING_BOX_RADIUS);
	}

#else
	sample = malloc(DIMENSION * sizeof(float));
	seed = rand_int(0, SAMPLES);
	for (i = 0; i < DIMENSION; i++){
		sample[i] = samples[seed + i];
	}
	landmarks_newLandmark(sample, landmarks, MAX_LANDMARKS, &freeList);
	for (i = 0; i < DIMENSION; i++){
		sample[i] = samples[seed + i];
	}
	landmarks_newLandmark(sample, landmarks, MAX_LANDMARKS, &freeList);
#endif

#if DEBUG
	fprintf(stderr, "findWinners.c: landmarks initialized.\n");
#endif

#if DEBUG
	fprintf(stderr,"findWinners.c: search Winners main loop.\n");
#endif


#if TIMING
	fprintf(stderr,"findWinners.c: started Timing.\n");
	t1 = clock();
#endif

	/** Algorithm Loop **/
	for(i = 0; i < SAMPLES; i++)
	{
		sample = &(samples[i]);
#if DEBUG
		fprintf(stderr, "findWinners.c: sample {%f, %f, %f}.\n", sample[0], sample[1], sample[2]);
#endif

		firstWinnerIndex = MAX_LANDMARKS;
		secondWinnerIndex = MAX_LANDMARKS;
		firstWinnerDistance = MAX_DISTANCE;
		secondWinnerDistance = MAX_DISTANCE;

#if DEBUG
		fprintf(stderr,"findWinners.c: start landmarks scan.\n");
#endif	
	
		/** Landmark scan loop **/
		for (j = 0; j < MAX_LANDMARKS; j++)
		{
#if DEBUG
			fprintf(stderr,"findWinners.c: landmark %d (Active: %s).\n", j, 
				(landmarks_isActive(j, landmarks, MAX_LANDMARKS) == 1) ? "YES" : "NO");
#endif	
			landmark = landmarks + j * DIMENSION;
			if (landmarks_isActiveCell(landmark))
			{
				distance = squaredEuclideanDistance(sample, landmark, DIMENSION);

				if (distance < firstWinnerDistance)
				{
					secondWinnerIndex = firstWinnerIndex;
					secondWinnerDistance = firstWinnerDistance;
					firstWinnerIndex = j;
					firstWinnerDistance = distance;
				}
				else if (distance < secondWinnerDistance)
				{
					secondWinnerIndex = j;
					secondWinnerDistance = distance;
				}
			} 
			else if (!landmarks_isActiveCell(landmark + 1))
				// EndOfData
				break;
		}
		
#if DEBUG
		fprintf(stderr,"findWinners.c: winner is %d with distance: %f - second is %d with distance: %f.\n",
			firstWinnerIndex, sqrt(firstWinnerDistance), secondWinnerIndex, sqrt(secondWinnerDistance));
#endif
	}

#if TIMING
	t2 = clock();
#endif

#if TIMING
	fprintf(stderr, "findWinnersCuda.c: %d samples and %d landmarks\n",
		SAMPLES, LOAD_LANDMARKS);
	fprintf(stderr, "findWinnersCuda.c: elapsed time (host): %fs\n",
		((double) (t2 - t1)) / CLOCKS_PER_SEC);
#endif

	waitUntilKeyPressed();
}
