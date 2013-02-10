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


// This enables CUDA-specific debug checks
// #define _DEBUG


#ifndef FIND_WINNERS_KERNEL
#define FIND_WINNERS_KERNEL
#endif

#include <float.h>
#include <math.h>
#include <cutil.h>

#include "landmarks.h"
#include "findWinnersKernel.h"


__constant__ int d_dimension = 0;
__constant__ int d_max_landmarks = 0;
__constant__ int d_max_samples = 0;

__constant__ float4 *d_landmarks = 0;
__constant__ float4 *d_samples = 0;
__constant__ int2 *d_indices = 0;
__constant__ float2 *d_distances = 0;


/* This version is for dimensions up to 4 */
__device__ void findWinnerTile4(int tile, float4 sample, int2 *bestMatching, float2 *bestDistance) {

	extern __shared__ float4 sharedLandmarks[];

	// fprintf(stderr, "findWinnersCuda.cu: findWinnerTile4 %d tile %d\n", blockIdx.x * blockDim.x + threadIdx.x, tile);

	if (tile + threadIdx.x < d_max_landmarks) {
		// Load landmark
		sharedLandmarks[threadIdx.x] = d_landmarks[tile + threadIdx.x];
	}
	else {
		landmarks_markInactiveCell(&sharedLandmarks[threadIdx.x].x);
	}

	__syncthreads();

	float tmp;
	float distance;

	for (int i = 0; i < blockDim.x; ++i) {
		tmp = sharedLandmarks[i].x;
		if (landmarks_isActiveCell(&tmp)) {
			tmp = sample.x - tmp;
			distance = tmp * tmp;
			tmp = sample.y - sharedLandmarks[i].y;
			distance += tmp * tmp;
			tmp = sample.z - sharedLandmarks[i].z;
			distance += tmp * tmp;
			tmp = sample.w - sharedLandmarks[i].w;
			distance += tmp * tmp;

			if (distance < (*bestDistance).x) {
				(*bestMatching).y = (*bestMatching).x;
				(*bestDistance).y = (*bestDistance).x;

				(*bestMatching).x = tile + i;
				(*bestDistance).x = distance;

				/*
				fprintf(stderr, "findWinnersCuda.cu: findWinnerTile4 %d tile %d bestMatching %d [%f,%f,%f,%f]->[%f,%f,%f,%f] distance %f\n", 
					blockIdx.x * blockDim.x + threadIdx.x, tile, (*bestMatching).x,
					sample.x, sample.y, sample.z, sample.w,
					sharedLandmarks[i].x, sharedLandmarks[i].y, sharedLandmarks[i].z, sharedLandmarks[i].w, tmp);
				*/
			} 
			else if (distance < (*bestDistance).y) {
				(*bestMatching).y = tile + i;
				(*bestDistance).y = distance;

				/*
				fprintf(stderr, "findWinnersCuda.cu: findWinnerTile4 %d tile %d secondBestMatching %d [%f,%f,%f,%f]->[%f,%f,%f,%f] distance %f\n", 
					blockIdx.x * blockDim.x + threadIdx.x, tile, (*bestMatching).y,
					sample.x, sample.y, sample.z, sample.w,
					sharedLandmarks[i].x, sharedLandmarks[i].y, sharedLandmarks[i].z, sharedLandmarks[i].w, tmp);
				*/
			}
		}
	}
}


/* This version is for dimensions up to 4 */
__global__ void findWinner4() {

	float4 sample;
	int2 bestMatching = {-1, -1};
	float2 bestDistance = {FLT_MAX, FLT_MAX};

	// fprintf(stderr, "findWinnersCuda.cu: findWinner4 %d load\n", blockIdx.x * blockDim.x + threadIdx.x);

	if (blockIdx.x * blockDim.x + threadIdx.x < d_max_samples) {
		// Load sample
		sample = d_samples[blockIdx.x * blockDim.x + threadIdx.x];

		// fprintf(stderr, "findWinnersCuda.cu: findWinner4 %d browse tiles\n", blockIdx.x * blockDim.x + threadIdx.x);

		// Browse tiles of landmarks
		for (int tile = 0; tile < d_max_landmarks; tile += blockDim.x) {
			findWinnerTile4(tile, sample, &bestMatching, &bestDistance);

			__syncthreads();
		}

		/*
		fprintf(stderr, "findWinnersCuda.cu: findWinner4 %d write indices %d %d %f %f\n",
			blockIdx.x * blockDim.x + threadIdx.x, bestMatching.x, bestMatching.y, bestDistance.x, bestDistance.y);
		*/

		// Write indices & distances
		d_indices[blockIdx.x * blockDim.x + threadIdx.x] = bestMatching;
		d_distances[blockIdx.x * blockDim.x + threadIdx.x] = bestDistance;
	}

	// fprintf(stderr, "findWinnersCuda.cu: findWinner4 %d completed\n", blockIdx.x * blockDim.x + threadIdx.x);
}

extern "C" {

int fwk_regsPerThread = 0;
int fwk_threadsPerBlock = 0;
int fwk_blocksPerGrid = 0;
int fwk_sharedMemSize = 0;


void fwk_allocateLandmarks() {
	float *d_landmarks = 0;
	int max_landmarks;

	CUDA_SAFE_CALL(cudaMemcpyFromSymbol(&max_landmarks, "d_max_landmarks", sizeof(int)));

	CUDA_SAFE_CALL(cudaMalloc((void **) &d_landmarks, max_landmarks * sizeof(float4)));
	CUDA_SAFE_CALL(cudaMemcpyToSymbol("d_landmarks", &d_landmarks, sizeof(float *)));
}

void fwk_allocateSamples() {
	float *d_samples = 0;
	int max_samples;

	CUDA_SAFE_CALL(cudaMemcpyFromSymbol(&max_samples, "d_max_samples", sizeof(int)));

	CUDA_SAFE_CALL(cudaMalloc((void **) &d_samples, max_samples * sizeof(float4)));
	CUDA_SAFE_CALL(cudaMemcpyToSymbol("d_samples", &d_samples, sizeof(float *)));
}

void fwk_allocateIndices() {
	int *d_indices = 0;
	int max_samples;

	CUDA_SAFE_CALL(cudaMemcpyFromSymbol(&max_samples, "d_max_samples", sizeof(int)));

	CUDA_SAFE_CALL(cudaMalloc((void **) &d_indices, max_samples * sizeof(int2)));
	CUDA_SAFE_CALL(cudaMemcpyToSymbol("d_indices", &d_indices, sizeof(int *)));
}

void fwk_allocateDistances() {
	float *d_distances = 0;
	int max_samples;

	CUDA_SAFE_CALL(cudaMemcpyFromSymbol(&max_samples, "d_max_samples", sizeof(int)));

	CUDA_SAFE_CALL(cudaMalloc((void **) &d_distances, max_samples * sizeof(float2)));
	CUDA_SAFE_CALL(cudaMemcpyToSymbol("d_distances", &d_distances, sizeof(float *)));
}

void fwk_setup(int max_landmarks, int max_samples, int dimension) {
	
	// CAUTION: make sure that REGS_PER_THREAD is up to date with .cubin
	fwk_regsPerThread = REGS_PER_THREAD;
	fwk_threadsPerBlock = THREADS_PER_BLOCK;
	fwk_sharedMemSize = fwk_threadsPerBlock * sizeof(float4);
	
	cudaDeviceProp deviceProp;

	// Consider the default device only
    CUDA_SAFE_CALL(cudaGetDeviceProperties(&deviceProp, 0));
	
	fwk_blocksPerGrid = max_samples / fwk_threadsPerBlock;
	fwk_blocksPerGrid = (fwk_blocksPerGrid * fwk_threadsPerBlock < max_samples) ?
		fwk_blocksPerGrid + 1 : fwk_blocksPerGrid;
	fwk_blocksPerGrid = (fwk_blocksPerGrid > deviceProp.maxGridSize[0]) ?
		deviceProp.maxGridSize[0] : fwk_blocksPerGrid;

	CUDA_SAFE_CALL(cudaMemcpyToSymbol("d_max_landmarks", &max_landmarks, sizeof(int)));
	CUDA_SAFE_CALL(cudaMemcpyToSymbol("d_max_samples", &max_samples, sizeof(int)));
	CUDA_SAFE_CALL(cudaMemcpyToSymbol("d_dimension", &dimension, sizeof(int)));

	/*
	max_landmarks = -1;
	max_samples = -1;
	dimension = -1;

	cudaMemcpyFromSymbol(&max_landmarks, "d_max_landmarks", sizeof(int));
	cudaMemcpyFromSymbol(&max_samples, "d_max_samples", sizeof(int));
	cudaMemcpyFromSymbol(&dimension, "d_dimension", sizeof(int));

	fprintf(stderr, "findWinnersCuda.cu: max_landmarks read %d\n", max_landmarks);
	fprintf(stderr, "findWinnersCuda.cu: max_samples read %d\n", max_samples);
	fprintf(stderr, "findWinnersCuda.cu: dimension read %d\n", dimension);
	*/

	fwk_allocateLandmarks();
	fwk_allocateSamples();
	fwk_allocateIndices();
	fwk_allocateDistances();
}


void fwk_setLandmarks(float *h_landmarks, int from, int to) {
	float *d_landmarks;

	CUDA_SAFE_CALL(cudaMemcpyFromSymbol(&d_landmarks, "d_landmarks", sizeof(float *)));

	CUDA_SAFE_CALL(cudaMemcpy(d_landmarks + from, h_landmarks, 
		to * sizeof(float4), cudaMemcpyHostToDevice));
}

void fwk_setSamples(float *h_samples, int from, int to) {
	float *d_samples;

	CUDA_SAFE_CALL(cudaMemcpyFromSymbol(&d_samples, "d_samples", sizeof(float *)));

	CUDA_SAFE_CALL(cudaMemcpy(d_samples + from, h_samples, 
		to * sizeof(float4), cudaMemcpyHostToDevice));
}

void fwk_getLandmarks(float *h_landmarks, int from, int to) {
	float *d_landmarks;

	CUDA_SAFE_CALL(cudaMemcpyFromSymbol(&d_landmarks, "d_landmarks", sizeof(float *)));

	CUDA_SAFE_CALL(cudaMemcpy(h_landmarks, d_landmarks + from,
		to * sizeof(float4), cudaMemcpyDeviceToHost));
}

void fwk_getSamples(float *h_samples, int from, int to) {
	float *d_samples;
	int dimension;

	CUDA_SAFE_CALL(cudaMemcpyFromSymbol(&d_samples, "d_samples", sizeof(float *)));
	CUDA_SAFE_CALL(cudaMemcpyFromSymbol(&dimension, "d_dimension", sizeof(int)));

	CUDA_SAFE_CALL(cudaMemcpy(h_samples, d_samples + from, 
		to * sizeof(float4), cudaMemcpyDeviceToHost));
}

void fwk_getIndices(int *h_indices, int from, int to) {
	int *d_indices;

	CUDA_SAFE_CALL(cudaMemcpyFromSymbol(&d_indices, "d_indices", sizeof(int *)));

	CUDA_SAFE_CALL(cudaMemcpy(h_indices, d_indices + from,
		to * sizeof(int2), cudaMemcpyDeviceToHost));
}

void fwk_getDistances(float *h_distances, int from, int to) {
	float *d_distances;

	CUDA_SAFE_CALL(cudaMemcpyFromSymbol(&d_distances, "d_distances", sizeof(float *)));

	CUDA_SAFE_CALL(cudaMemcpy(h_distances, d_distances + from,
		to * sizeof(float2), cudaMemcpyDeviceToHost));
}

void fwk_findWinners() {
	int max_samples, max_landmarks;

	CUDA_SAFE_CALL(cudaMemcpyFromSymbol(&max_landmarks, "d_max_landmarks", sizeof(int)));
	CUDA_SAFE_CALL(cudaMemcpyFromSymbol(&max_samples, "d_max_samples", sizeof(int)));

	dim3 block(fwk_threadsPerBlock, 1, 1);
	dim3 grid(fwk_blocksPerGrid, 1, 1);

	/*
	fprintf(stderr, "findWinnersCuda.cu: grid {%d, %d, %d}, block {%d, %d, %d}, sharedMemSize %d\n",
		grid.x, grid.y, grid.z,
		block.x, block.y, block.z,
		fwk_sharedMemSize);
	*/

	findWinner4<<<grid, block, fwk_sharedMemSize>>>();

	// check if kernel invocation generated an error
    CUT_CHECK_ERROR("Kernel execution failed");
}

void fwk_threadSyncronize() {
	CUDA_SAFE_CALL(cudaThreadSynchronize());
}

void fwk_releaseLandmarks() {
	float *d_landmarks;

	CUDA_SAFE_CALL(cudaMemcpyFromSymbol(&d_landmarks, "d_landmarks", sizeof(float *)));
	CUDA_SAFE_CALL(cudaFree(d_landmarks));

	d_landmarks = 0;
	CUDA_SAFE_CALL(cudaMemcpyToSymbol("d_landmarks", &d_landmarks, sizeof(float *)));
}

void fwk_releaseSamples() {
	float *d_samples;

	CUDA_SAFE_CALL(cudaMemcpyFromSymbol(&d_samples, "d_samples", sizeof(float *)));
	CUDA_SAFE_CALL(cudaFree(d_samples));

	d_samples = 0;
	CUDA_SAFE_CALL(cudaMemcpyToSymbol("d_samples", &d_samples, sizeof(float *)));
}

void fwk_releaseIndices() {
	int *d_indices;

	CUDA_SAFE_CALL(cudaMemcpyFromSymbol(&d_indices, "d_indices", sizeof(int *)));
	CUDA_SAFE_CALL(cudaFree(d_indices));

	d_indices = 0;
	CUDA_SAFE_CALL(cudaMemcpyToSymbol("d_indices", &d_indices, sizeof(int *)));
}

void fwk_releaseDistances() {
	float *d_distances;

	CUDA_SAFE_CALL(cudaMemcpyFromSymbol(&d_distances, "d_distances", sizeof(float *)));
	CUDA_SAFE_CALL(cudaFree(d_distances));

	d_distances = 0;
	CUDA_SAFE_CALL(cudaMemcpyToSymbol("d_distances", &d_distances, sizeof(float *)));
}

void fwk_release() {
	int max_landmarks = 0;
	int max_samples = 0;
	int dimension = 0;

	fwk_releaseLandmarks();
	fwk_releaseSamples();
	fwk_releaseIndices();
	fwk_releaseDistances();

	CUDA_SAFE_CALL(cudaMemcpyToSymbol("d_max_landmarks", &max_landmarks, sizeof(int)));
	CUDA_SAFE_CALL(cudaMemcpyToSymbol("d_max_samples", &max_samples, sizeof(int)));
	CUDA_SAFE_CALL(cudaMemcpyToSymbol("d_dimension", &dimension, sizeof(int)));
}


} // extern "C"

