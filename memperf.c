#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mymalloc.h"
#define NUM_POINTERS 20

const int amt = 5000;
void* pointers[20];

void randomIteration() {
	for (int n = 0; n < amt; n++) {
		int randGen = rand() % 3;
		int pointerIndex = rand()%20;
		int size = (rand()%256)+1;
		if(randGen == 0){
			printf("malloc is being called\n");
			pointers[pointerIndex] = mymalloc(size);
		}
		if(randGen == 2){
			printf("realloc is being called\n");
			pointers[pointerIndex] = myrealloc(pointers[pointerIndex], size);
		}
		if(randGen ==1){
			printf("free is being called\n");
			myfree(pointers[pointerIndex]);
		}
	}

}

void resetPointers() {
	for (int n = 0; n < NUM_POINTERS; n++)
		pointers[n] = NULL;
}

int main(int argc, char* argv[]) {
	time_t begin;
	time_t end;
	time_t seed;
	srand((unsigned) time(&seed));

	double firstFitUtil;
	double firstFitTP;

	double secondFitUtil;
	double secondFitTP;

	double bestFitUtil;
	double bestFitTP;

	myinit(0);
	//printf("SEED: %lld\n", (long long) seed);
	begin = time(NULL);
	randomIteration();
	end = time(NULL);
	firstFitTP = (double)amt / (double)(end - begin);
	firstFitUtil= utilization();
	mycleanup();
	resetPointers();

	myinit(1);
	//printf("SEED: %lld\n", (long long) seed);
	begin = time(NULL);
	randomIteration();
	end = time(NULL);
	secondFitTP = (double)amt / (double)(end - begin);
	secondFitUtil = utilization();
	mycleanup();
	resetPointers();

	myinit(2);
	//printf("SEED: %lld\n", (long long) seed);
	begin = time(NULL);
	randomIteration();
	end = time(NULL);
	bestFitTP = (double)amt / (double)(end - begin);
	bestFitUtil = utilization();
	mycleanup();
	resetPointers();

	printf("First fit throughput: %f ops/sec\n", firstFitTP);
	printf("First fit utilization: %f\n", firstFitUtil);
	printf("Second fit throughput: %f ops/sec\n", secondFitTP);
	printf("Second fit utilization: %f\n", secondFitUtil);
	printf("Best fit throughput: %f ops/sec\n", bestFitTP);
	printf("Best fit utilization: %f\n", bestFitUtil);

	return(0);

}
