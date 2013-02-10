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
#include "findWinnersKernel.h"

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
	// Device memory
	float *landmarks, *landmark;
	int freeList;
	float *samples;
	int *indices;
	float *distances;
	
	int index;

	// Validation only
	float *backup;

	int i, j;


#if TIMING
	clock_t t1;
	clock_t t2;
#endif

	srand((unsigned int) time(NULL));


#if DEBUG
	fprintf(stderr, "findWinnersCuda.c: warming Up for execution...\n");
#endif
	/** Warming up **/
	samples = loadSamples();

	landmarks_setSpaceDimension(DIMENSION);

	landmarks = (float *) malloc(MAX_LANDMARKS * DIMENSION * sizeof(float));
	landmarks_initialize(landmarks, MAX_LANDMARKS, &freeList);

	indices = (int *) malloc(SAMPLES * SIZEOF_INDEX);
	distances = (float *) malloc(SAMPLES * SIZEOF_DISTANCE);

#if DEBUG
	fprintf(stderr, "findWinnersCuda.c: system ready, variables initialized.\n");
#endif

	/** Insert a Landmark at the position of the first casual sample, in order to have at least one iteration **/
#ifdef LOAD_LANDMARKS
#if DEBUG
	fprintf(stderr, "findWinnersCuda.c: initializing list with %d samples...\n", LOAD_LANDMARKS);
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
	fprintf(stderr, "findWinnersCuda.c: landmarks initialized.\n");
#endif

	fwk_setup(MAX_LANDMARKS, SAMPLES, DIMENSION);

	fprintf(stderr, "findWinnersCuda.c: kernel setup.\n");
	fprintf(stderr, "findWinnersCuda.c: registers per thread: %d.\n", fwk_regsPerThread);
	fprintf(stderr, "findWinnersCuda.c: threads per block: %d.\n", fwk_threadsPerBlock);
	fprintf(stderr, "findWinnersCuda.c: blocks per grid: %d.\n", fwk_blocksPerGrid);
	fprintf(stderr, "findWinnersCuda.c: shared memory size: %d.\n", fwk_sharedMemSize);

	fwk_setLandmarks(landmarks, 0, MAX_LANDMARKS);

	fprintf(stderr, "findWinnersCuda.c: landmarks copied to device.\n");

	backup = (float *) malloc(MAX_LANDMARKS * DIMENSION * sizeof(float));
	fwk_getLandmarks(backup, 0, MAX_LANDMARKS);

	fprintf(stderr, "findWinnersCuda.c: backup landmarks copied back to host.\n");
	fprintf(stderr, "findWinnersCuda.c: landmarks[0] %f d_landmarks[0] %f.\n",
		landmarks[0], backup[0]);

	fwk_setSamples(samples, 0, SAMPLES);

	fprintf(stderr, "findWinnersCuda.c: samples copied to device.\n");

#if DEBUG
	fprintf(stderr,"findWinnersCuda.c: search Winners main loop.\n");
#endif

#if TIMING
	fprintf(stderr,"findWinnersCuda.c: started Timing.\n");
	t1 = clock();
#endif

	/** Kernel invocation **/
	fwk_findWinners();
	fwk_threadSyncronize();

#if TIMING
	t2 = clock();
#endif

#if TIMING
	fprintf(stderr, "findWinnersCuda.c: %d samples and %d landmarks\n",
		SAMPLES, LOAD_LANDMARKS);
	fprintf(stderr, "findWinnersCuda.c: elapsed time (host): %fs\n",
		((double) (t2 - t1)) / CLOCKS_PER_SEC);
#endif

	fwk_getIndices(indices, 0, SAMPLES);
	fprintf(stderr, "findWinnersCuda.c: indices retrieved.\n");

	fwk_getDistances(distances, 0, SAMPLES);
	fprintf(stderr, "findWinnersCuda.c: distances retrieved.\n");

	i = 0;
	fprintf(stderr, "findWinnersCuda.c: index %d sample [%f,%f,%f,%f]:\n",
		i, samples[i*DIMENSION*sizeof(float)], samples[i*DIMENSION*sizeof(float) + 1],
		samples[i*DIMENSION*sizeof(float) + 2], samples[i*DIMENSION*sizeof(float) + 3]);
	fprintf(stderr, "findWinnersCuda.c: index %d bestMatching %d bestDistance %f secondBestMatching %d secondBestDistance %f\n",
		i, indices[i*SIZEOF_INDEX], distances[i*SIZEOF_DISTANCE], indices[i*SIZEOF_INDEX+1], distances[i*SIZEOF_DISTANCE+1]);

	i = 1;
	fprintf(stderr, "findWinnersCuda.c: index %d sample [%f,%f,%f,%f]:\n",
		i, samples[i*DIMENSION*sizeof(float)], samples[i*DIMENSION*sizeof(float) + 1],
		samples[i*DIMENSION*sizeof(float) + 2], samples[i*DIMENSION*sizeof(float) + 3]);
	fprintf(stderr, "findWinnersCuda.c: index %d bestMatching %d bestDistance %f secondBestMatching %d secondBestDistance %f\n",
		i, indices[i*SIZEOF_INDEX], distances[i*SIZEOF_DISTANCE], indices[i*SIZEOF_INDEX+1], distances[i*SIZEOF_DISTANCE+1]);

	i = 2;
	fprintf(stderr, "findWinnersCuda.c: index %d sample [%f,%f,%f,%f]:\n",
		i, samples[i*DIMENSION*sizeof(float)], samples[i*DIMENSION*sizeof(float) + 1],
		samples[i*DIMENSION*sizeof(float) + 2], samples[i*DIMENSION*sizeof(float) + 3]);
	fprintf(stderr, "findWinnersCuda.c: index %d bestMatching %d bestDistance %f secondBestMatching %d secondBestDistance %f\n",
		i, indices[i*SIZEOF_INDEX], distances[i*SIZEOF_DISTANCE], indices[i*SIZEOF_INDEX+1], distances[i*SIZEOF_DISTANCE+1]);

	fwk_release();
	fprintf(stderr, "findWinnersCuda.c: kernel memory released.\n");

	waitUntilKeyPressed();
}
