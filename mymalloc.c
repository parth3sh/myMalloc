#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>

static int fitAlgo;
static char* memory;
#define HEAPSIZE 1024*1024
#define MEMSIZE 8
#define FREE 123321
#define ALLOCATED 321123
#define DOUBLE_FREE 679976

void myinit(int allocAlgo);
void* mymalloc(size_t size);
void myfree(void* ptr);

typedef struct memBlock {
	int free;
	size_t size;
	struct memBlock *nextFree;
	struct memBlock *prevFree;
} memBlock;

static memBlock* freeHead;
static memBlock* lastUsed;

int main(int argc, char* argv[]) {
	myinit(1);
	//int* ptr1 = (int*)mymalloc(12);
	void* ptr2 = mymalloc(5);
	//char* ptr3 = (char*)mymalloc(35);
	//void* ptr4 = mymalloc(1);
	//uintptr_t p1 = (uintptr_t)(ptr1);
	uintptr_t p2 = (uintptr_t)(ptr2);
	//uintptr_t p3 = (uintptr_t)(ptr3);
	//uintptr_t p4 = (uintptr_t)(ptr4);
	//printf("p1 = %p\n", ptr1);
	printf("p2 = %p\n", ptr2);
	//printf("p3 = %p\n", ptr3);
	//printf("p4 = %p\n", ptr4);
	//printf("p1 = %ld\n", p1%8);
	printf("p2 = %ld\n", p2%8);
	//printf("p3 = %ld\n", p3%8);
	//printf("p4 = %ld\n", p4%8);
	myfree(ptr2);
	myfree(ptr2);
	//myfree(tester);
}



void myinit(int allocAlgo){
	memory = malloc(HEAPSIZE);
	fitAlgo = allocAlgo;
	freeHead = (void*)memory;
	freeHead->size = HEAPSIZE;
	freeHead->free = FREE;
}

void split(memBlock* ptr, size_t size){
	memBlock *extra=(void*)((void*)ptr+size);
	extra->size=(ptr->size)-size;
	extra->free=FREE;
	extra->nextFree = ptr->nextFree;
	extra->prevFree = ptr->prevFree;
	ptr->size=size;
	ptr->free=ALLOCATED;
	ptr->nextFree = extra;
	lastUsed = extra;
 }

void* firstFit(size_t size){
	memBlock* ptr = freeHead;
	memBlock* prev = NULL;
	while(ptr!=NULL){

		if(ptr->size == size){
			ptr->free = ALLOCATED;
			lastUsed = ptr->nextFree;
			if (prev == NULL) {
				freeHead = ptr->nextFree;
				if (freeHead)
					freeHead->prevFree = NULL;
			} else {
				prev->nextFree = ptr->nextFree;
				if (ptr->nextFree)
					ptr->nextFree->prevFree = prev;
			}
			return (void*) (++ptr);			
		}

		else if(ptr->size > size){
			split(ptr, size);
			if (prev == NULL) {
				freeHead = ptr->nextFree;
				if (freeHead)
					freeHead->prevFree = NULL;
			} else {
				prev->nextFree = ptr->nextFree;
				if (ptr->nextFree)
					ptr->nextFree->prevFree = prev;
			}
			return (void*) (++ptr);
		}

		prev = ptr;
		ptr = ptr->nextFree;
	}
	return NULL;
}

void* nextFit(size_t size){
	memBlock* ptr = lastUsed;
	memBlock* prev = NULL;
	while(ptr!=NULL){
		if(ptr->size == size){
			ptr->free = ALLOCATED;
			lastUsed = ptr->nextFree;
			if (prev == NULL) {
				freeHead = ptr->nextFree;
				if (freeHead)
					freeHead->prevFree = NULL;
			} else {
				prev->nextFree = ptr->nextFree;
				if (ptr->nextFree)
					ptr->nextFree->prevFree = prev;
			}
			return (void*) (++ptr);			
		}
		else if(ptr->size > size){
			split(ptr, size);
			if (prev == NULL) {
				freeHead = ptr->nextFree;
				if (freeHead)
					freeHead->prevFree = NULL;
			} else {
				prev->nextFree = ptr->nextFree;
				if (ptr->nextFree)
					ptr->nextFree->prevFree = prev;
			}
			return (void*) (++ptr);
		}
		prev = ptr;
		ptr = ptr->nextFree;
	}
	ptr = freeHead;
	prev = NULL;

	while(ptr!=NULL && ptr!=lastUsed){
		if(ptr->size == size){
			ptr->free = ALLOCATED;
			lastUsed = ptr->nextFree;
			if (prev == NULL) {
				freeHead = ptr->nextFree;
				if (freeHead)
					freeHead->prevFree = NULL;
			} else {
				prev->nextFree = ptr->nextFree;
				if (ptr->nextFree)
					ptr->nextFree->prevFree = prev;
			}
			return (void*) (++ptr);			
		}
		else if(ptr->size > size){
			split(ptr, size);
			if (prev == NULL) {
				freeHead = ptr->nextFree;
				if (freeHead)
					freeHead->prevFree = NULL;
			} else {
				prev->nextFree = ptr->nextFree;
				if (ptr->nextFree)
					ptr->nextFree->prevFree = prev;
			}
			return (void*) (++ptr);
		}
		prev = ptr;
		ptr = ptr->nextFree;
	}
	return NULL;
}


void *bestFit(size_t size) {
	memBlock* ptr = freeHead;
	memBlock* prev = NULL;
	memBlock* best = NULL;
	int dis = INT_MAX;
	while(ptr!=NULL){

		if(ptr->size == size){
			ptr->free = ALLOCATED;
			lastUsed = ptr->nextFree;
			if (prev == NULL) {
				freeHead = ptr->nextFree;
				if (freeHead)
					freeHead->prevFree = NULL;
			} else {
				prev->nextFree = ptr->nextFree;
				if (ptr->nextFree)
					ptr->nextFree->prevFree = prev;
			}
			return (void*) (++ptr);			
		}
		else if(ptr->size > size && (ptr->size-size < dis)){
			dis = ptr->size - size;
			best = ptr;
		}
		prev = ptr;
		ptr = ptr->nextFree;
	}
	if (best) {
		ptr = best;
		split(ptr, size);
		if (best->prevFree)
			best->prevFree->nextFree = best->nextFree;
		else
			freeHead = best->nextFree;
		if (best->nextFree)
			best->nextFree->prevFree = best->prevFree;
		return (void*) (++ptr);
	}
	return NULL;
}

void* mymalloc(size_t size){
	size_t tempSize = MEMSIZE;
	if (size > MEMSIZE){
		tempSize = MEMSIZE * ((size + (MEMSIZE-1)) / MEMSIZE);
	}
	size = tempSize + sizeof(memBlock);
	void* ptr = NULL;
	if(fitAlgo == 0){
		ptr = firstFit(size);	
	}

	//secondfit 
	else if(fitAlgo == 1){
		ptr = nextFit(size);
	}
	
	// Best Fit
	else if(fitAlgo == 2){
		ptr = bestFit(size);
	}
	return ptr;

}

void freeHelper(memBlock* ptr) {
	memBlock* prev = NULL;
	memBlock* curr = freeHead;
	uintptr_t addy = (uintptr_t)(ptr);
	uintptr_t currAddy;
	ptr->free = DOUBLE_FREE;
	while (curr) {
		currAddy = (uintptr_t)(curr);
		if (currAddy - addy > 0) {
			ptr->prevFree = prev;
			ptr->nextFree = curr;
			if (prev == NULL) {
				freeHead = ptr;
			} else {
				prev->nextFree = ptr;
			}
			curr->prevFree = ptr;
			break;
		}
		prev = curr;
		curr = curr->nextFree;
	}
	if (!curr) {
		ptr->prevFree = prev;
		ptr->nextFree = curr;
		prev->nextFree = ptr;
	}
	return;
}

void myfree(void* ptr) {
	if (!ptr)
		return;
	uintptr_t lowerBound = (uintptr_t)((void*)memory + sizeof(memBlock));
	uintptr_t upperBound = (uintptr_t)((void*)memory + HEAPSIZE - MEMSIZE);
	uintptr_t add = (uintptr_t)(ptr);
	if (add < lowerBound || add >= upperBound) {
		printf("error: not a heap pointer\n");
		return;
	}
	if (add % 8) {
		printf("error: not a malloced address\n");
		printf("because address not divisible by 8\n");
		return;
	}
	memBlock* nPtr = (memBlock*)((void*)ptr - sizeof(memBlock));
	printf("nPtr->free = %d\n", nPtr->free);
	if (nPtr->free != FREE && nPtr->free != ALLOCATED && nPtr->free != DOUBLE_FREE) {
		printf("error: not a malloced address\n");
		printf("because mid block\n");
		return;
	}
	if (nPtr->free == FREE) {
		printf("error: not a malloced address\n");
		printf("because it's equal to free\n");
		return;
	}
	if (nPtr->free == DOUBLE_FREE) {
		printf("error: double free\n");
		return;
	}
	freeHelper(nPtr);
	printf("Succ\n");
	// 2. coalesce
	// 3. Profit

	return;
}
