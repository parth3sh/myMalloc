#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>

static int fitAlgo;
static char* memory;
#define HEAPSIZE 1024*1024
#define MEMSIZE 8

void myinit(int allocAlgo);
void* mymalloc(size_t size);

typedef struct memBlock {
	int free;
	size_t size;
	struct memBlock *nextFree;
	struct memBlock *prevFree;
} memBlock;

static memBlock* freeHead;
static memBlock* lastUsed;

int main(int argc, char* argv[]) {
	myinit(0);
	int* ptr1 = (int*)mymalloc(12);
	void* ptr2 = mymalloc(5);
	char* ptr3 = (char*)mymalloc(35);
	void* ptr4 = mymalloc(7);
	//uintptr_t p1 = (uintptr_t)(ptr1);
	//uintptr_t p2 = (uintptr_t)(ptr2);
	//uintptr_t p3 = (uintptr_t)(ptr3);
	//uintptr_t p4 = (uintptr_t)(ptr4);
	printf("p1 = %p\n", ptr1);
	printf("p2 = %p\n", ptr2);
	printf("p3 = %p\n", ptr3);
	printf("p4 = %p\n", ptr4);
}



void myinit(int allocAlgo){
	memory = malloc(HEAPSIZE);
	fitAlgo = allocAlgo;
	freeHead = (void*)memory;
	freeHead->size = HEAPSIZE;
}

void split(memBlock* ptr, size_t size){
	memBlock *extra=(void*)((void*)ptr+size);
	extra->size=(ptr->size)-size;
	extra->free=1;
	extra->nextFree = ptr->nextFree;
	extra->prevFree = ptr->prevFree;
	ptr->size=size;
	ptr->free=0;
	ptr->nextFree = extra;
	lastUsed = extra;
 }

void* firstFit(size_t size){
	memBlock* ptr = freeHead;
	while(ptr!=NULL){

		if(ptr->size == size){
			ptr->free = 0;
			lastUsed = ptr->nextFree;
			return (void*) (++ptr);			
		}

		else if(ptr->size > size){
			split(ptr, size);
			return (void*) (++ptr);
		}

		ptr = ptr->nextFree;
	}
	return NULL;
}

void* nextFit(size_t size){
	memBlock* ptra = lastUsed;
	while(ptra!=NULL){
		if(ptra->size == size){
			ptra->free = 0;
			lastUsed = ptra->nextFree;
			return (void*) ptra;			
		}
		else if(ptra->size > size){
			split(ptra, size);
			return (void*) ptra;
		}
		ptra = ptra->nextFree;
	}
	ptra = freeHead;

	while(ptra!=NULL && ptra!=lastUsed){
		if(ptra->size == size){
			ptra->free = 0;
			lastUsed = ptra->nextFree;
			return (void*) ptra;			
		}
		else if(ptra->size > size){
			split(ptra, size);
			return (void*) ptra;
		}
		ptra = ptra->nextFree;
	}
	return NULL;
}


void *bestFit(size_t size) {
	memBlock* ptr = freeHead;
	memBlock* best = NULL;
	int dis = INT_MAX;
	while(ptr!=NULL){

		if(ptr->size == size){
			ptr->free = 0;
			lastUsed = ptr->nextFree;
			return (void*) ptr;			
		}
		else if(ptr->size > size && (ptr->size-size < dis)){
			dis = ptr->size - size;
			best = ptr;
		}
		ptr = ptr->nextFree;
	}
	if (ptr) {
		ptr = best;
		split(ptr, size);
		return ptr;
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
	
	else if(fitAlgo == 2){
		ptr = bestFit(size);
	}
	return ptr;

}

