#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "mymalloc.h"


static int fitAlgo;
static char* memory;
//static int usedMem;
//static int usedSpace;


static memBlock* freeHead;
static memBlock* lastUsed;


void freeiteration() {
	//printf("new file tester\n");
	memBlock *tempPtr = freeHead;
	printf("\n");
	printf("NEW LIST ITERATION \n");
	while(tempPtr!=NULL){
		printf("THIS ADDRESS: %zd | NEXT FREE: %zd | PREV FREE: %zd | SIZE: %d | FREE: %d\n", (uintptr_t)tempPtr - (uintptr_t)memory, (uintptr_t)tempPtr->nextFree - (uintptr_t)memory,(uintptr_t)tempPtr->prevFree - (uintptr_t)memory, tempPtr->size, tempPtr->free);
		if(tempPtr == tempPtr->nextFree){
			printf("NEXT FREE EQUALS ITSELF. TERMINATING EARLY\n");
			exit(1);
		}
		tempPtr = tempPtr -> nextFree;
	}
	printf("\n");
}


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
	memBlock *extra=(memBlock*)((void*)ptr+size);
	size_t diff = (ptr->size) - size;
	extra->size=diff;
	extra->free=FREE;
	extra->payload = 0;
	extra->nextFree = ptr->nextFree;
	extra->prevFree = ptr->prevFree;
	if(ptr->prevFree != NULL){
		ptr->prevFree->nextFree = extra;
	}
	else{
		freeHead = extra;
	}
	if(ptr->nextFree){
		ptr->nextFree->prevFree = extra;
	}
	ptr->size=size;
	ptr->free=ALLOCATED;
	lastUsed = extra;
	ptr->nextFree = NULL;
	ptr->prevFree = NULL;
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
			ptr->nextFree = NULL;
			ptr->prevFree = NULL;
			return (void*) (++ptr);			
		}


		else if(ptr->size > size && ptr->size - size >= sizeof(memBlock) + MEMSIZE) {
			split(ptr, size);
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

	if (ptr != NULL) {
		prev = ptr->prevFree;
	}

	while(ptr!=NULL && (void*)ptr <= (void*)memory + HEAPSIZE - MEMSIZE - sizeof(memBlock)){
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
			ptr->nextFree = NULL;
			ptr->prevFree = NULL;
			return (void*) (++ptr);			
		}
		else if(ptr->size > size && ptr->size - size >= sizeof(memBlock) + MEMSIZE){
			split(ptr, size);
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
		ptr->nextFree = NULL;
		ptr->prevFree = NULL;
			return (void*) (++ptr);			
		}
		else if(ptr->size > size && ptr->size - size >= sizeof(memBlock) + MEMSIZE){
			split(ptr, size);
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
	while(ptr!=NULL && (void*)ptr <= (void*)memory + HEAPSIZE - MEMSIZE - sizeof(memBlock)){

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
			ptr->nextFree = NULL;
			ptr->prevFree = NULL;
			return (void*) (++ptr);			
		}
		else if(ptr->size > size && (ptr->size-size < dis) && (ptr->size-size >= sizeof(memBlock) + MEMSIZE)){
			dis = ptr->size - size;
			best = ptr;
		}
		prev = ptr;
		ptr = ptr->nextFree;
		if (ptr && (ptr->free != FREE && ptr->free != DOUBLE_FREE)) {
			//printf("in the best fit loop, ptr->free = %d\n", ptr->free);
		}
	}
	if (best) {
		ptr = best;
		split(ptr, size);
		return (void*) (++ptr);
	}
	return NULL;
}

void* mymalloc(size_t size){
	int originalSize = size;
	size_t tempSize = MEMSIZE;
	memBlock* temp;
	//Make size 8 bit aligned and add it to memblock size (32 bytes)
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

	//set original payload of block for util purposes. 
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
	ptr->free = DOUBLE_FREE;
	while (curr && (uintptr_t)curr < (uintptr_t)memory + HEAPSIZE) {
		currAddy = (uintptr_t)((void*)curr);
		long long int diff = (long long int)(currAddy) - (long long int)(addy);
		//printf("currAddy = %zd, addy = %zd, nex:%zd\n", currAddy, addy, (uintptr_t)curr->nextFree);
		if ((diff) > 0) {
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
	if (!curr && prev) {
		ptr->prevFree = prev;
		ptr->nextFree = curr;
		prev->nextFree = ptr;
	}
	else if(!curr && !prev){
		freeHead = ptr;
		ptr->prevFree = NULL;
		ptr->nextFree = NULL;
	}
	return;
}


void coal(memBlock* ptr){
    //Both sides are allocated and/or null
	uintptr_t prevAndSize = 0;
	uintptr_t currAndSize = 0;

	if (ptr->prevFree != NULL) {
		prevAndSize = (uintptr_t)(ptr->prevFree) + (ptr->prevFree)->size;
	}

	if (ptr->nextFree != NULL) {
		currAndSize = (uintptr_t)(ptr) + ptr->size;
	}

	//memBlock* tester = ptr;


	if((ptr->prevFree == NULL || (prevAndSize != (uintptr_t)ptr)) && (ptr->nextFree == NULL || currAndSize != (uintptr_t)(ptr->nextFree))){
		return;
	}
    //Left side is allocated, right is free
	else if((ptr->prevFree == NULL || (prevAndSize != (uintptr_t)ptr)) && (currAndSize == (uintptr_t)(ptr-> nextFree))){
		ptr->size = ptr->size + ptr->nextFree->size;
		memBlock* temp = ptr->nextFree;
        ptr->nextFree = ptr->nextFree->nextFree;
		if (ptr->nextFree != NULL) {
			ptr->nextFree->prevFree = ptr;
		}
		if (temp == lastUsed)
			lastUsed = ptr;
		temp->free = -1;
        return;
	}
    //Right allocated, left free
	else if(((prevAndSize == (uintptr_t)ptr)) && (ptr->nextFree == NULL || currAndSize!= (uintptr_t)(ptr->nextFree))){
		memBlock* temp = ptr;
        ptr->prevFree->size = ptr->prevFree->size + ptr->size;
        ptr->prevFree->nextFree = ptr->nextFree;
		if(ptr->nextFree!=NULL){
			ptr->nextFree->prevFree = ptr->prevFree;
		}
		if (temp == lastUsed)
			lastUsed = temp->prevFree;
		temp->free = -1;
        return;
    }
    //Both sides free
    else{
		memBlock* temp1 = ptr;
		memBlock* temp2 = ptr->nextFree;
		ptr->size = ptr->size + ptr->nextFree->size;
        ptr->nextFree = ptr->nextFree->nextFree;
		if (ptr->nextFree != NULL) {
			ptr->nextFree->prevFree = ptr;
		}
        ptr->prevFree->size = ptr->prevFree->size + ptr->size;
        ptr->prevFree->nextFree = ptr->nextFree;
		if(ptr->nextFree!=NULL){
			ptr->nextFree->prevFree = ptr->prevFree;
		}
		if (temp2 == lastUsed)
			lastUsed = temp1->prevFree;
		temp1->free = -1;
		temp2->free = -1;
        return;
    }

}




void myfree(void* ptr) {
	if (!ptr)
		return;
	uintptr_t lowerBound = (uintptr_t)((void*)memory + sizeof(memBlock));
	uintptr_t upperBound = (uintptr_t)((void*)memory + HEAPSIZE - MEMSIZE);
	uintptr_t add = (uintptr_t)(ptr);
	if (add < lowerBound || add > upperBound) {
		printf("error: not a heap pointer\n");
		printf("in my free because out of bounds\n");
		return;
	}
	if (add % 8) {
		printf("error: not a malloced address\n");
		printf("in my free because address isn't divisible by 8\n");
		return;
	}
	memBlock* nPtr = (memBlock*)((void*)ptr - sizeof(memBlock));
	//printf("nPtr->free = %d\n", nPtr->free);
	if (nPtr->free != FREE && nPtr->free != ALLOCATED && nPtr->free != DOUBLE_FREE) {
		printf("error: not a malloced address\n");
		printf("in my free because garbage\n");
		return;
	}
	if (nPtr->free == FREE) {
		printf("error: not a malloced address\n");
		printf("in my free because marked as free block\n");
		return;
	}
	if (nPtr->free == DOUBLE_FREE) {
		printf("error: double free\n");
		printf("in my free\n");
		return;
	}
	freeHelper(nPtr);
	//printf("Succ\n");
	// 2. coalesce
	coal(nPtr);
	// 3. Profit
	return;
}

void* myrealloc(void* ptr, size_t size) {
	uintptr_t lowerBound = (uintptr_t)((void*)memory + sizeof(memBlock));
	uintptr_t upperBound = (uintptr_t)((void*)memory + HEAPSIZE - MEMSIZE - sizeof(memBlock));
	uintptr_t add = (uintptr_t)(ptr);
	if (add < lowerBound || add > upperBound) {
		printf("error: not a heap pointer\n");
		return NULL;
	}
	if (add % 8) {
		printf("error: not a malloced address\n");
		return NULL;
	}
	int originalSize = size;
	if (!ptr) {
		if (size == 0)
			return NULL;
		return mymalloc(size);
	}

	if (size == 0) {
		myfree(ptr);
		ptr = NULL;
		return NULL;
	}

	size_t tempSize = MEMSIZE;
	if (size > MEMSIZE) {
		tempSize = MEMSIZE * ((size + (MEMSIZE-1)) / MEMSIZE);
	}
	size = tempSize + sizeof(memBlock);


	memBlock* m = (memBlock*)((void*)ptr - sizeof(memBlock));
	if (m->free != FREE && m->free != ALLOCATED && m->free != DOUBLE_FREE) {
		return NULL;
	}
	if (m->free == FREE || m->free == DOUBLE_FREE) {
		return NULL;
	}
	memBlock* next = NULL;
	if ((uintptr_t)m + m->size < (uintptr_t)memory + HEAPSIZE){
		next = (memBlock*)((void*)m + m->size);
	}
	if (m->size == size) {
		return ptr;
	}
	//case where its smaller but not small enough to split...

	if (m->size > size && m->size-size >= sizeof(memBlock) + MEMSIZE) {
		memBlock* newFreeBlock = (void*)m + size;
		newFreeBlock->size = m->size - size;
		newFreeBlock->payload = 0;
		newFreeBlock->free = FREE;
		m->size = size;
		m->payload = originalSize;
		freeHelper(newFreeBlock);
		coal(newFreeBlock);
		return ptr;
	}
	else if (m->size > size && m->size-size <= sizeof(memBlock) + MEMSIZE){
		return ptr;
	}
	else if ((next == NULL) || (next->free == ALLOCATED) || (m->size + next->size < size)) {
		void* newPtr = mymalloc(size-sizeof(memBlock));

		if (!newPtr) {
			return newPtr;
		}
		memmove(newPtr, ptr, m->size - sizeof(memBlock));
		((memBlock*)((void*)(newPtr) - sizeof(memBlock)))->payload = originalSize;
		myfree(ptr);
		return newPtr;
	} 
	else if (next->free == FREE || next->free == DOUBLE_FREE) {
		size_t combinedSize = m->size + next->size;

		//In place cases:

		//if m and next size is same as size then no split necessary just merge
		if(combinedSize == size || combinedSize - size < sizeof(memBlock) + MEMSIZE){
			if(next->prevFree != NULL){
				next->prevFree->nextFree = next -> nextFree;
			}
			else{
				freeHead = next ->nextFree;
			}
			if(next->nextFree != NULL){
				next->nextFree->prevFree = next->prevFree;
			}
			if(lastUsed == next){
				lastUsed = next->nextFree;
			}
			m->size = combinedSize;
			m->payload = originalSize;
			return ptr;
		}

		if(combinedSize > size && combinedSize - size > sizeof(memBlock) + MEMSIZE){
			int neededSize = size - m->size;
			next->size = next->size - neededSize;
			m->size = size;			
			m->payload = originalSize;
			if (next->prevFree) {
				next->prevFree->nextFree = (void*)next + neededSize;
			} else {
				freeHead = (void*)next + neededSize;
			}
			if (next->nextFree) {
				next->nextFree->prevFree = (void*)next + neededSize;
			}
			if (lastUsed == next) {
				lastUsed = (void*)next + neededSize;
			}
			memmove((void*)next+neededSize, next, sizeof(memBlock));
			return ptr;
		}

	}

	return NULL;
	
}

void mycleanup() {
	free(memory);
	freeHead = NULL;
	lastUsed = NULL;
}

double utilization() {
	memBlock* curr = (void*)memory;
	memBlock* lastAllocatedBlock = NULL;
	double memoryUsed = 0;
	double spaceUsed;
	void* end = NULL;
	while (((void*)curr) <= (((void*)memory + HEAPSIZE - MEMSIZE - sizeof(memBlock)))) {
		if (curr->free != FREE && curr->free != DOUBLE_FREE && curr->free != ALLOCATED) {
			printf("entered garbage data at location: %zd\n", (uintptr_t)curr-(uintptr_t)memory);
			exit(1);
			break;
		}

		if (curr->free == ALLOCATED) {
			memoryUsed += curr->payload;
			lastAllocatedBlock = curr;
		}

		curr = (memBlock*)((void*)curr + curr->size);
	}
	if (!lastAllocatedBlock) {
		spaceUsed = 0;
	} else {
		end = (void*)lastAllocatedBlock + lastAllocatedBlock->size;
		spaceUsed = end - (void*)memory;
	}
	double ans = memoryUsed / spaceUsed;
	return ans;
}
