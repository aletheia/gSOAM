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


#include "utils.h"

float squaredEuclideanDistance(float* v1, float *v2, int world_DIMENSION)
{
	int i;
	float distance = 0;
	for(i = 0; i < world_DIMENSION; i++)
	{
		distance += (v1[i] - v2[i]) * (v1[i] - v2[i]);
	}
	return distance;
}


void wait (int seconds)
{
  clock_t endwait;
  endwait = clock() + seconds * CLOCKS_PER_SEC ;
  while (clock() < endwait) {}
}

void waitUntilKeyPressed(){
	// scanf("%s");
	printf("\nPress ENTER to exit...\n");
	fflush(stdout);
	fflush(stderr);
	getchar();
}


float rand_float(float min_val, float max_val){
	float value;
	value = min_val + ((float) rand() / RAND_MAX) * (max_val - min_val);
	return value;
}

int rand_int(int min_val, int max_val){
	return (int) (rand_float((float) min_val, (float) max_val));
}

