#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mymalloc.h"
/*
To-Do:
1. Make random calls to mymalloc, myrealloc, and myfree (1 mil)
2. Time how long it takes to finish making all calls
3. Use utilization()
4. mycleanup()

- For number 1, how to choose between mymalloc, myrealloc, and myfree?
	- Assign 0 to mymalloc, 1 to myrealloc, and 2 to myfree
	- Pick random number in interval [0,2]

- For timing, get time before starting test for one fit, time after, and subtract
- Throughput = num ops / time


*/

int main(int argc, char* argv[]) {
	myinit(1);
	time_t seed;
	int amt = 10000;
	//srand((unsigned) time(&seed));
	//srand(1637272616);
	printf("SEED: %lld", (long long) seed);

	//srand(1637269614);
	//srand(1637342748);
	srand(1637346510);
	void* pointers[20];
	if(!pointers[0]){
		printf("NULL");
	}
	for( int n = 0 ; n < 2*amt; n++ ) {
		printf("iteration # = %d\n", n);
		int randGen = rand() % 2;
		int pointerIndex = rand()%20;
		int size = (rand()%256)+1;
		//int randGen = 0;
		if(randGen == 0){
			printf("malloc is being called\n");
			pointers[pointerIndex] = mymalloc(size);
		}
		if(randGen == 2){
			printf("realloc is being called\n");
			myrealloc(pointers[pointerIndex], size);
		}
		if(randGen ==1){
			printf("free is being called\n");
			myfree(pointers[pointerIndex]);
		}
	}
	//for( int n = 0 ; n < amt; n++ ) {
	//	printf("iteration # = %d\n", n);
	//	//int randGen = rand() % 3;
	//	int pointerIndex = rand()%10;
	//	int size = (rand()%256)+1;
	//	int randGen = 2;
	//	if(randGen == 0){
	//		printf("malloc is being called\n");
	//		pointers[pointerIndex] = mymalloc(size);
	//	}
	//	if(randGen == 1){
	//		printf("realloc is being called\n");
	//		myrealloc(pointers[pointerIndex], size);
	//	}
	//	if(randGen ==2){
	//		printf("free is being called\n");
	//		myfree(pointers[pointerIndex]);
	//	}
	//}
	//for( int n = 0 ; n < amt; n++ ) {
	//	printf("iteration # = %d\n", n);
	//	//int randGen = rand() % 3;
	//	int pointerIndex = rand()%10;
	//	int size = (rand()%256)+1;
	//	int randGen = 1;
	//	if(randGen == 0){
	//		printf("malloc is being called\n");
	//		pointers[pointerIndex] = mymalloc(size);
	//	}
	//	if(randGen == 1){
	//		printf("realloc is being called\n");
	//		myrealloc(pointers[pointerIndex], size);
	//	}
	//	if(randGen ==2){
	//		printf("free is being called\n");
	//		myfree(pointers[pointerIndex]);
	//	}
	//}

	double utilNum = utilization();
	printf("Utilization: %f\n", utilNum);
	mycleanup();


	return(0);

}
