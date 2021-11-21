#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "mymalloc.h"


static int fitAlgo;
static char* memory;
static int usedMem;
static int usedSpace;


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
	printf("size of entire block is %ld\n", ptr->size);
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
	if(ptr->prevFree != NULL && (uintptr_t)ptr->prevFree > (uintptr_t)ptr && (uintptr_t)ptr->prevFree < (uintptr_t)memory + HEAPSIZE){
		printf("this one aint either\n");
		ptr->prevFree->nextFree = extra;
	}
	else{
		freeHead = extra;
	}
	if(ptr->nextFree && (uintptr_t)ptr->nextFree > (uintptr_t)ptr && (uintptr_t)ptr->nextFree < (uintptr_t)memory + HEAPSIZE){
		ptr->nextFree->prevFree = extra;
	}
	ptr->size=size;
	printf("setting old block = allocated and its size = %d\n", ptr->size);
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
			printf("(first fit) SAME SIZE MALLLOC, THE SIZE IS: %d\n", ptr->size);
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
	while(ptr!=NULL && (void*)ptr <= (void*)memory + HEAPSIZE - MEMSIZE - sizeof(memBlock)){
		printf("still in the first loop\n");
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
	printf("i made it out of the first loop for second fit\n");
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
			printf("in the best fit loop, ptr->free = %d\n", ptr->free);
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
	printf("(mymalloc)Post aligned new size = %ld\n", size);
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
	printf("setting ptr = double free\n");
	ptr->free = DOUBLE_FREE;
	while (curr && (uintptr_t)curr < (uintptr_t)memory + HEAPSIZE) {
		currAddy = (uintptr_t)((void*)curr);
		long long int diff = (long long int)(currAddy) - (long long int)(addy);
		printf("currAddy = %zd, addy = %zd, nex:%zd\n", currAddy, addy, (uintptr_t)curr->nextFree);
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
	printf("entered coalesce function\n");
    //Both sides are allocated and/or null
	uintptr_t prevAndSize = 0;
	uintptr_t prevToCurr = 0;
	uintptr_t currAndSize = 0;
	uintptr_t currToNext = 0;

	if (ptr->prevFree != NULL) {
		prevAndSize = (uintptr_t)(ptr->prevFree) + (ptr->prevFree)->size;
		//prevToCurr = MEMSIZE * ((prevAndSize + (MEMSIZE-1)) / MEMSIZE);
	}

	if (ptr->nextFree != NULL) {
		currAndSize = (uintptr_t)(ptr) + ptr->size;
		//currToNext = MEMSIZE * ((currAndSize + (MEMSIZE-1)) / MEMSIZE);
	}

	printf("prevAndSize = %zd, currAndSize = %zd\n", prevAndSize, currAndSize);
	printf("address of beginning = %zd, address of ptr = %zd\n", (uintptr_t)((void*)memory), (uintptr_t)ptr);
	memBlock* tester = ptr;
	if (ptr->prevFree == NULL) {
		printf("prevFree == NULL\n");
		printf("now it's time to see if left block is actually prevFree\n");
		while (1) {
			tester = (memBlock*)((void*)tester - 8);
			if ((void*)tester < (void*)memory) {
				printf("went too far behind\n");
				break;
			}
			if (tester->free == ALLOCATED || tester->free == FREE || tester->free == DOUBLE_FREE) {
				printf("tester->free = %d\n", tester->free);
				break;
			}
		}
	}


	printf("current pointer address = %zd, prev p address = %zd, next p address = %zd\n", (uintptr_t)(ptr), (uintptr_t)(ptr->prevFree), (uintptr_t)(ptr->nextFree));
	if((ptr->prevFree == NULL || (prevAndSize!= (uintptr_t)ptr)) && (ptr->nextFree == NULL || currAndSize != (uintptr_t)(ptr->nextFree))){
		printf("both sides are allocated\n");
		return;
	}
    //Left side is allocated, right is free
	else if((ptr->prevFree == NULL || (prevAndSize != (uintptr_t)ptr)) && (currAndSize == (uintptr_t)(ptr-> nextFree))){
		printf("left is allocated, right is free\n");
		ptr->size = ptr->size + ptr->nextFree->size;
        ptr->nextFree = ptr->nextFree->nextFree;
		if (ptr->nextFree != NULL) {
			ptr->nextFree->prevFree = ptr;
		}
        return;
	}
    //Right allocated, left free
	else if(((prevAndSize == (uintptr_t)ptr)) && (ptr->nextFree == NULL || currAndSize!= (uintptr_t)(ptr->nextFree))){
		printf("right is allocated, left is free\n");
        ptr->prevFree->size = ptr->prevFree->size + ptr->size;
        ptr->prevFree->nextFree = ptr->nextFree;
		if(ptr->nextFree!=NULL){
			ptr->nextFree->prevFree = ptr->prevFree;
		}
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
		if(ptr->nextFree!=NULL){
			ptr->nextFree->prevFree = ptr->prevFree;
		}
		printf("in da end\n");
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
	memBlock *tempPtr = freeHead;
	printf("\n");
	printf("NEW LIST ITERATION \n");
	while(tempPtr!=NULL){
		printf("THIS ADDRESS: %zd | NEXT FREE: %zd | PREV FREE: %zd | SIZE: %d | FREE: %d\n", (uintptr_t)tempPtr - (uintptr_t)memory, (uintptr_t)tempPtr->nextFree - (uintptr_t)memory,(uintptr_t)tempPtr->prevFree - (uintptr_t)memory, tempPtr->size, tempPtr->free);
		if(tempPtr == tempPtr->nextFree){
			exit(1);
		}
		tempPtr = tempPtr -> nextFree;
	}
	printf("\n");
	return;
}

void* myrealloc(void* ptr, size_t size) {
	printf("realloc size = %ld\n", size);
	uintptr_t lowerBound = (uintptr_t)((void*)memory + sizeof(memBlock));
	uintptr_t upperBound = (uintptr_t)((void*)memory + HEAPSIZE - MEMSIZE);
	uintptr_t add = (uintptr_t)(ptr);
	if (add < lowerBound || add > upperBound) {
		printf("error: not a heap pointer\n");
		return NULL;
	}
	if (add % 8) {
		printf("error: not a malloced address\n");
		printf("because address not divisible by 8\n");
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
		printf("because mid block\n");
		return NULL;
	}
	printf("size = %ld, tempSize = %ld, mSIZE: %ld\n", size, tempSize, m->size);	
	printf("M ADDRESS: %zd\n", (uintptr_t)m);
	printf("End of heap: %zd\n", (uintptr_t)((void*)memory + HEAPSIZE));
	printf("M CALC = %zd\n", (uintptr_t)m + m->size);
	printf("heap CALC = %zd\n", (uintptr_t)memory + HEAPSIZE);
	memBlock* next = NULL;
	if ((uintptr_t)m + m->size < (uintptr_t)memory + HEAPSIZE){
		next = (memBlock*)((void*)m + m->size);
		printf("Next size is: %d", next->size);
	}
	if (m->size == size) {
		printf("size is da same\n");
		return ptr;
	}
	//case where its smaller but not small enough to split...

	if (m->size > size && m->size-size > sizeof(memBlock) + MEMSIZE) {
		printf("needs to be split\n");
		split(m, size);
		m->payload = originalSize;
		return ptr;
	}
	else if (m->size > size && m->size-size <= sizeof(memBlock) + MEMSIZE){
		printf("NOT SMALL ENOUGH TO SPLIT!");
		return ptr;
	}
	else if ((next == NULL) || (next->free == ALLOCATED) || (m->size + next->size < size)) {
		printf("cant be resized in place\n");
		myfree(ptr);
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
		size_t combinedSize = m->size + next->size;
		printf("m->size = %d, next->size = %d,  and combinedSize = %ld\n", m->size, next->size, combinedSize);
		//if (combinedSize >= size)
		//	printf("combined size is greater than size\n");
		//else
		//	printf("combined isnt greater\n");
		//if (m->size > size)
		//	printf("m->size > size\n");
		//else
		//	printf("m->size <= size\n");
		//if (combinedSize-size >= sizeof(memBlock) + MEMSIZE)
		//	printf("combinedSize-size >= sizeof(memBlock) + MEMSIZE\n");
		//else
		//	printf("combinedSize-size < sizeof(memBlock) + MEMSIZE\n");

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
		//if m and next size is greater than size, then split the next block and merge into split

		if(combinedSize > size && combinedSize - size > sizeof(memBlock) + MEMSIZE){

			int neededSize = size - m->size;
			memmove(next+neededSize, next, sizeof(memBlock));
			next->size = next->size - neededSize;
			m->size = size;			
			m->payload = originalSize;
			return ptr;
		}

		// if (combinedSize >= size && m->size > size && combinedSize-size >= sizeof(memBlock) + MEMSIZE) {
		// 	printf("i should be in here\n");
		// 	int newSize = m->size - size;
		// 	m->size = combinedSize;
		// 	m->nextFree = next->nextFree;
		// 	if (m->nextFree) {
		// 		m->nextFree->prevFree = m;
		// 	}
		// 	m->payload = originalSize;
		// 	split(m, size);
		// }
		// else if (combinedSize == size) {
		// 	m->free = ALLOCATED;
		// 	m->payload = originalSize;
		// 	m->size += m->nextFree->size;
		// 	m->nextFree = m->nextFree->nextFree;
		// 	if (m->nextFree) {
		// 		m->nextFree->prevFree = m;
		// 	}
		// 	printf("resized in place - same size\n");
		// 	return ptr;
		// }
		// else if (combinedSize > size && combinedSize-size  >= sizeof(memBlock) + MEMSIZE) {
		// 	m->size += next->size;
		// 	m->nextFree = next->nextFree;
		// 	if (m->nextFree) {
		// 		m->nextFree->prevFree = m;
		// 	}
		// 	m->payload = originalSize;
		// 	split(m, size);
		// 	printf("resized in place - greater size\n");
		// 	return ptr;
		// }
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
	int s = 0;
	double spaceUsed;
	void* end = NULL;
	int start = 0;
	while (((void*)curr) <= (((void*)memory + HEAPSIZE - MEMSIZE - sizeof(memBlock)))) {

		printf("curr->free = %d\n", curr->free);
		printf("curr->size = %d\n", curr->size);

		if (curr->free != FREE && curr->free != DOUBLE_FREE && curr->free != ALLOCATED) {
			printf("entered garbage data at location: %zd and mem is %zd\n", (uintptr_t)curr, (uintptr_t)memory);
			//exit(1);
			break;
		}
		if (curr->free == ALLOCATED) {
			printf("found an allocated block\n");
			memoryUsed += curr->payload;
			lastAllocatedBlock = curr;
		}
		printf("add of curr = %zd\n", (uintptr_t)curr);
		printf("add of end = %zd\n", (uintptr_t)((void*)memory + HEAPSIZE - MEMSIZE - sizeof(memBlock)));
		printf("curr->size = %d\n", curr->size);
		s += curr->size;
		curr = (memBlock*)((void*)curr + curr->size);
		start = 1;
	}
	printf("out of da loop\n");
	memBlock* pp = freeHead;
	while (pp) {
		if (pp->free != FREE && pp->free != DOUBLE_FREE) {
			if (pp->free == ALLOCATED) {
				printf("a free block is apparently allocated. TERMINATING EARLY\n");
				// exit(1);
			}
			printf("garbage data in free list. TERMINATING EARLY\n");
			// exit(1);
		}
		pp = pp->nextFree;
	}
	printf("out of da free loop\n");
	if (!lastAllocatedBlock) {
		printf("it is null bruv\n");
		spaceUsed = 0;
	} else {
		end = (void*)lastAllocatedBlock + lastAllocatedBlock->size;
		spaceUsed = end - (void*)memory;
	}
	printf("memory used = %f\n", memoryUsed);
	printf("space used = %f\n", spaceUsed);
	double ans = memoryUsed / spaceUsed;
	printf("ratio = %f\n", ans);
	printf("s = %d\n", s);
	return ans;
}
