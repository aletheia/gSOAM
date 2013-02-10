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


#ifndef FIND_WINNERS_KERNEL_H
#define FIND_WINNERS_KERNEL_H

#define MAX_SPACE_DIMENSION 4
#define SIZEOF_INDEX (2 * sizeof(int))
#define SIZEOF_DISTANCE (2 * sizeof(float))

// CAUTION! This value has to be kept up to date with the .cubin output
#define REGS_PER_THREAD 16
#define THREADS_PER_BLOCK 512

#ifndef FIND_WINNERS_KERNEL

extern int fwk_regsPerThread;
extern int fwk_threadsPerBlock;
extern int fwk_blocksPerGrid;
extern int fwk_sharedMemSize;

void fwk_setup(int max_landmarks, int max_samples, int dimension);

void fwk_allocateSamples();
void fwk_allocateLandmarks();
void fwk_allocateIndices();
void fwk_allocateDistances();

void fwk_setSamples(float *h_samples, int from, int to);
void fwk_setLandmarks(float *h_landmarks, int from, int to);

void fwk_getSamples(float *h_samples, int from, int to);
void fwk_getLandmarks(float *h_landmarks, int from, int to);
void fwk_getIndices(int *h_indices, int from, int to);
void fwk_getDistances(float *h_distances, int from, int to);

void fwk_findWinners();

void fwk_threadSyncronize();

void fwk_release();

void fwk_releaseSamples();
void fwk_releaseLandmarks();
void fwk_releaseIndices();
void fwk_releaseDistances();

#endif // #ifndef FIND_WINNERS_KERNEL

#endif // #ifndef FIND_WINNERS_KERNEL_H