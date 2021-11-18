#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "mymalloc.h"


static int fitAlgo;
static char* memory;



static memBlock* freeHead;
static memBlock* lastUsed;




void myinit(int allocAlgo){
	memory = malloc(HEAPSIZE);
	fitAlgo = allocAlgo;
	freeHead = (void*)memory;
	freeHead->size = HEAPSIZE;
	freeHead->free = FREE;
	freeHead->nextFree = NULL;
	freeHead->prevFree = NULL;
	freeHead->payload = 0;
}

void split(memBlock* ptr, size_t size){
	printf("the size being passed into split = %ld\n", size);
	memBlock *extra=(memBlock*)((void*)ptr+size);
	size_t diff = (ptr->size) - size;
	//diff = max(diff, sizeof(memBlock)+MEMSIZE);
	extra->size=diff;
	printf("setting new block = free and its size = %d\n", extra->size);
	extra->free=FREE;
	extra->payload = 0;
	extra->nextFree = ptr->nextFree;
	extra->prevFree = ptr->prevFree;
	ptr->size=size;
	printf("setting old block = allocated and its size = %d\n", ptr->size);
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

		else if(ptr->size > size && ptr->size - size >= sizeof(memBlock) + MEMSIZE) {
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
		else if(ptr->size > size && ptr->size - size >= sizeof(memBlock) + MEMSIZE){
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
		else if(ptr->size > size && ptr->size - size >= sizeof(memBlock) + MEMSIZE){
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
		else if(ptr->size > size && (ptr->size-size < dis) && (ptr->size-size >= sizeof(memBlock) + MEMSIZE)){
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
	int originalSize = size;
	size_t tempSize = MEMSIZE;
	memBlock* temp;
	if (size > MEMSIZE){
		tempSize = MEMSIZE * ((size + (MEMSIZE-1)) / MEMSIZE);
	}
	size = tempSize + sizeof(memBlock);
	printf("new size = %ld\n", size);
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


	if (ptr) {
		temp = (memBlock*)((void*)ptr - sizeof(memBlock));
		temp->payload = originalSize;
	}

	return ptr;

}

void freeHelper(memBlock* ptr) {
	memBlock* prev = NULL;
	memBlock* curr = freeHead;
	uintptr_t addy = (uintptr_t)((void*)ptr);
	uintptr_t currAddy;
	currAddy = (uintptr_t)(curr);
	printf("setting ptr = double free\n");
	ptr->free = DOUBLE_FREE;
	while (curr) {
		currAddy = (uintptr_t)((void*)curr);
		long long int diff = (long long int)(currAddy) - (long long int)(addy);
		printf("currAddy = %zd, addy = %zd\n", currAddy, addy);
		if ((diff) > 0) {
			printf("currAddy = %zd, addy = %zd\n", currAddy, addy);
			printf("diff = %lld\n", diff);
			ptr->prevFree = prev;
			printf("address of ptr->prevFree = %zd\n", (uintptr_t)ptr->prevFree);
			ptr->nextFree = curr;
			if (prev == NULL) {
				freeHead = ptr;
				printf("does freehead get updated?\n");
			} else {
				printf("i am in here lol\n");
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

void coal(memBlock* ptr){
	printf("entered coalesce function\n");
    //Both sides are allocated and/or null
	uintptr_t prevAndSize = 0;
	uintptr_t prevToCurr = 0;
	uintptr_t currAndSize = 0;
	uintptr_t currToNext = 0;

	if (ptr->prevFree != NULL) {
		prevAndSize = (uintptr_t)(ptr->prevFree) + (ptr->prevFree)->size;
		prevToCurr = MEMSIZE * ((prevAndSize + (MEMSIZE-1)) / MEMSIZE);
	}

	if (ptr->nextFree != NULL) {
		currAndSize = (uintptr_t)(ptr) + ptr->size;
		currToNext = MEMSIZE * ((currAndSize + (MEMSIZE-1)) / MEMSIZE);
	}

	printf("prevAndSize = %zd, currAndSize = %zd\n", prevAndSize, currAndSize);


	printf("current pointer address = %zd, prev p address = %zd, next p address = %zd\n", (uintptr_t)(ptr), (uintptr_t)(ptr->prevFree), (uintptr_t)(ptr->nextFree));
	if((ptr->prevFree == NULL || (prevToCurr!= (uintptr_t)ptr)) && (ptr->nextFree == NULL || currToNext != (uintptr_t)(ptr->nextFree))){
		printf("both sides are allocated\n");
		return;
	}
    //Left side is allocated, right is free
	else if((ptr->prevFree == NULL || (prevToCurr != (uintptr_t)ptr)) && (currToNext == (uintptr_t)(ptr-> nextFree))){
		printf("left is allocated, right is free\n");
		ptr->size = ptr->size + ptr->nextFree->size;
        ptr->nextFree = ptr->nextFree->nextFree;
		if (ptr->nextFree != NULL) {
			ptr->nextFree->prevFree = ptr;
		}
        return;
	}
    //Right allocated, left free
	else if(((prevToCurr == (uintptr_t)ptr)) && (ptr->nextFree == NULL || currToNext!= (uintptr_t)(ptr->nextFree))){
		printf("right is allocated, left is free\n");
        ptr->prevFree->size = ptr->prevFree->size + ptr->size;
        ptr->prevFree->nextFree = ptr->nextFree;
        return;
    }
    //Both sides free
    else{
		printf("both sides free\n");
		ptr->size = ptr->size + ptr->nextFree->size;
		printf("num 1\n");
        ptr->nextFree = ptr->nextFree->nextFree;
		printf("num 2\n");
		if (ptr->nextFree != NULL) {
			printf("in da conditional\n");
			printf("address = %zd\n", (uintptr_t)(ptr->nextFree));
			ptr->nextFree->prevFree = ptr;
			printf("num 3\n");
		}
        ptr->prevFree->size = ptr->prevFree->size + ptr->size;
		printf("num 4\n");
        ptr->prevFree->nextFree = ptr->nextFree;
		printf("in da end\n");
        return;
    }

}

void myfree(void* ptr) {
	if (!ptr)
		return;
	uintptr_t lowerBound = (uintptr_t)((void*)memory + sizeof(memBlock));
	uintptr_t upperBound = (uintptr_t)((void*)memory + HEAPSIZE - MEMSIZE - sizeof(memBlock));
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
	coal(nPtr);
	// 3. Profit

	return;
}

void* myrealloc(void* ptr, size_t size) {
	uintptr_t lowerBound = (uintptr_t)((void*)memory + sizeof(memBlock));
	uintptr_t upperBound = (uintptr_t)((void*)memory + HEAPSIZE - MEMSIZE - sizeof(memBlock));
	uintptr_t add = (uintptr_t)(ptr);
	if (add < lowerBound || add >= upperBound) {
		printf("error: not a heap pointer\n");
		return NULL;
	}
	if (add % 8) {
		printf("error: not a malloced address\n");
		printf("because address not divisible by 8\n");
		return NULL;
	}
	int originalSize = size;
	printf("i am in realloc\n");
	if (!ptr) {
		if (size == 0)
			return NULL;
		return mymalloc(size);
	}

	if (size == 0) {
		myfree(ptr);
		return NULL;
	}

	size_t tempSize = MEMSIZE;
	if (size > MEMSIZE) {
		tempSize = MEMSIZE * ((size + (MEMSIZE-1)) / MEMSIZE);
	}
	size = tempSize + sizeof(memBlock);
	printf("size = %ld, tempSize = %ld\n", size, tempSize);

	memBlock* m = (memBlock*)((void*)ptr - sizeof(memBlock));
	printf("M ADDRESS: %zd\n", (uintptr_t)m);
	printf("End of heap: %zd\n", (uintptr_t)((void*)memory + HEAPSIZE));
	memBlock* next = NULL;
	if ((void*)m + m->size < (void*)memory + HEAPSIZE)
		next = (memBlock*)((void*)m + m->size);
	if (m->size == size) {
		printf("size is da same\n");
		return ptr;
	}

	if (m->size > size && m->size-size >= sizeof(memBlock) + MEMSIZE) {
		printf("needs to be split\n");
		split(m, size);
		m->payload = originalSize;
		return ptr;
	}
	else if ((next == NULL) || (next->free == ALLOCATED) || (m->size + next->size < size)) {
		printf("cant be resized in place\n");
		//myfree(ptr);
		void* newPtr = mymalloc(size-sizeof(memBlock));
		if (!newPtr) {
			printf("couldnt find da block\n");
			return newPtr;
		}
		memmove(newPtr, ptr, m->size - sizeof(memBlock));
		((memBlock*)((void*)(newPtr) - sizeof(memBlock)))->payload = originalSize;
		return newPtr;
	} 
	else if (next->free == FREE || next->free == DOUBLE_FREE) {
		size_t combinedSize = m->size + m->nextFree->size;
		if (combinedSize == size) {
			m->free = ALLOCATED;
			m->payload = originalSize;
			m->size += m->nextFree->size;
			m->nextFree = m->nextFree->nextFree;
			if (m->nextFree) {
				m->nextFree->prevFree = m;
			}
			printf("resized in place - same size\n");
			return ptr;
		}
		else if (combinedSize > size && next->size-(size-(m->size)) >= sizeof(memBlock) + MEMSIZE) {
			split(next, size-(m->size));
			m->size += next->size;
			m->nextFree = next->nextFree;
			if (m->nextFree) {
				m->nextFree->prevFree = m;
			}
			m->payload = originalSize;
			printf("resized in place - greater size\n");
			return ptr;
		}
	}
	printf("ended up here\n");

	return NULL;
	
}

void mycleanup() {
	free(memory);
}

double utilization() {
	printf("in da util\n");
	memBlock* curr = (void*)memory;
	memBlock* lastAllocatedBlock = NULL;
	double memoryUsed = 0;
	double spaceUsed;
	while (((void*)curr) < (((void*)memory + HEAPSIZE - MEMSIZE - sizeof(memBlock)))) {
		printf("curr->free = %d\n", curr->free);
		if (curr->free == ALLOCATED) {
			printf("found an allocated block\n");
			memoryUsed += curr->payload;
			lastAllocatedBlock = curr;
		}
		printf("add of curr = %zd\n", (uintptr_t)curr);
		printf("add of end = %zd\n", (uintptr_t)((void*)memory + HEAPSIZE - MEMSIZE - sizeof(memBlock)));
		printf("curr->size = %d\n", curr->size);
		curr = (memBlock*)((void*)curr + curr->size);
	}
	printf("out of da loop\n");
	if (!lastAllocatedBlock)
		printf("it is null bruv\n");
	void* end = (void*)lastAllocatedBlock + lastAllocatedBlock->size;
	spaceUsed = end - (void*)memory;
	printf("memory used = %f\n", memoryUsed);
	printf("space used = %f\n", spaceUsed);
	double ans = memoryUsed / spaceUsed;
	printf("ratio = %f\n", ans);
	return ans;
}
