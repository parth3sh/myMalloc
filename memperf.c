#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mymalloc.h"

#define NUM_POINTERS 10
void* pointers[NUM_POINTERS];

void randomOperations(int amt) {
	for (int n = 0; n < amt; n++) {
		int randGen = rand() % 3;
		int pointerIndex = rand() % NUM_POINTERS;
		int size = rand() % 257;
		if (randGen == 0) {
			pointers[pointerIndex] = mymalloc(size);
		}
		else if (randGen == 1) {
			myrealloc(pointers[pointerIndex], size);
		}
		else if (randGen == 2) {
			myfree(pointers[pointerIndex]);
			//pointers[pointerIndex] = NULL;
		}
	}
}

int main(int argc, char* argv[]) {
	time_t seed;
	int amt = 2500000;
	srand((unsigned) time(&seed));

	double firstFitUtil = 0.0;
	double secondFitUtil = 0.0;
	double bestFitUtil = 0.0;

	double timeSpentFirstFit = 0.0;
	double firstFitThroughput = 0.0;

	double timeSpentSecondFit = 0.0;
	double secondFitThroughput = 0.0;

	double timeSpentBestFit = 0.0;
	double bestFitThroughput = 0.0;

	time_t begin;
	time_t end;

	myinit(0);
	begin = time(NULL);
	randomOperations(amt);
	end = time(NULL);
	timeSpentFirstFit = (double)(end - begin);
	firstFitThroughput = (double)amt / timeSpentFirstFit;

	firstFitUtil = utilization();
	mycleanup();

	myinit(1);
	begin = time(NULL);
	randomOperations(amt);
	end = time(NULL);
	timeSpentSecondFit = (double)(end - begin);
	secondFitThroughput = (double)amt / timeSpentSecondFit;

	secondFitUtil = utilization();
	mycleanup();


	myinit(2);
	begin = time(NULL);
	randomOperations(amt);
	end = time(NULL);
	timeSpentBestFit = (double)(end - begin);
	bestFitThroughput = (double)amt / timeSpentBestFit;

	bestFitUtil = utilization();
	mycleanup();

	printf("First fit throughput: %f ops/sec\n", firstFitThroughput);
	printf("First fit utilization: %f\n", firstFitUtil);

	printf("Second fit throughput: %f ops/sec\n", secondFitThroughput);
	printf("Second fit utilization: %f\n", secondFitUtil);

	printf("Best fit throughput: %f ops/sec\n", bestFitThroughput);
	printf("Best fit utilization: %f\n", bestFitUtil);

	return 0;

}
