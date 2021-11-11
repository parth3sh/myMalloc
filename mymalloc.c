#include <stdio.h>
#include <stdlib.h>

static int fitAlgo;
static char* memory;
#define HEAPSIZE 1024*1024
#define MEMSIZE 8

void myinit(int allocAlgo);

typedef struct memBlock {
	int free;
	size_t size;
	struct memBlock *nextFree;
	struct memBlock *prevFree;
} memBlock;

static memBlock* freeHead;
static memBlock* lastBlock;

int main(int argc, char* argv[]) {
	printf("%d", sizeof(memBlock));
	myinit(2);
	for(int n = 0; n < 1024*1024; n++){
		memory[n] = 'p';
	}
	printf("Test: %c %d\n", memory[5], fitAlgo);

}



void myinit(int allocAlgo){
	memory = malloc(HEAPSIZE);
	fitAlgo = allocAlgo;
	blockHead->free = 1;
	blockHead->size = HEAPSIZE;
	blockHead->nextBlock = NULL;
	blockHead->prevBlock = NULL;

	blockHead->nextFree = NULL;
	blockHead->prevFree = NULL;
	freeHead = blockHead;
}

void* mymalloc(size_t size){
	tempSize = MEMSIZE;
	if (size > MEMSIZE){
		tempSize = MEMSIZE * ((size + (MEMSIZE-1)) / MEMSIZE);
	}
	size = tempSize + sizeof(memBlock);
	if(fitAlgo == 0){
		firstFit(size);	
	}

	//secondfit 
	else if(fitAlgo == 1){
		nextFit(size);
	}
	
	else if(fitAlgo == 2){
		bestFit(size);
}

}

void* firstFit(size_t size){
	memBlock* ptr = freeHead;
	while(ptr!=NULL){
		if(ptr->size < size){
			continue;
		}
		if(ptr->size == size){
			ptr->free = 0;
			return (void*) ptr;			
		}
		else if(ptr->size > size){
			split(ptr, size);
			return (void*) ptr;
		}
		ptr = ptr->nextFree;
	}
	return NULL;
}

void split(memBlock* ptr, size_t size){
	memBlock *extra=(void*)((void*)ptr+size);
	extra->size=(ptr->size)-size;
	extra->free=1;
	extra->nextBlock=ptr->nextBlock;
	extra->prevBlock = ptr;
	extra->nextFree = ptr->nextFree;
	extra->prevFree = ptr->prevFree;
	ptr->size=size;
	ptr->free=0;
	ptr->nextBlock=extra;
	ptr->nextFree = extra;
 }

void* nextFit(size_t size){
	memBlock* ptra = lastUsed;
	while(ptra!=NULL){
		if(ptra->size < size){
			continue;
		}
		if(ptra->size == size){
			ptra->free = 0;
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
		if(ptra->size < size){
			continue;
		}
		if(ptra->size == size){
			ptra->free = 0;
			return (void*) ptra;			
		}
		else if(ptra->size > size){
			split(ptra, size);
			return (void*) ptra;
		}
		ptra = ptra->nextFree;
	}
}
