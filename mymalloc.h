#ifndef MYMALLOC_H
#define MYMALLOC_H

#define HEAPSIZE 1024*1024
#define MEMSIZE 8
#define FREE 123321
#define ALLOCATED 321123
#define DOUBLE_FREE 679976
#include <stddef.h>

typedef struct memBlock {
	int free;
	int size;
	int payload;
	struct memBlock *nextFree;
	struct memBlock *prevFree;
} memBlock;

void myinit(int allocAlgo);
void* mymalloc(size_t size);
void myfree(void* ptr);
void* myrealloc(void* ptr, size_t size);
void mycleanup();
double utilization();

#endif
