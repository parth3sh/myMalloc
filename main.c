#include "mymalloc.h"
#include <stddef.h>

int main(int argc, char* argv[]) {
	myinit(0);
	void* ptr1 = mymalloc(25);
	utilization();
	//int* ptr1 = (int*)mymalloc(50);
	//uintptr_t ogAddy = (uintptr_t)ptr1;
	//ptr1[0] = 1;
	//ptr1[1] = 2;
	//ptr1[2] = 3;
	//ptr1[3] = 4;
	//printf("sizeof(int) = %ld\n", sizeof(int));
	//printf("sizeof(size_t) = %ld\n", sizeof(size_t));
	//printf("sizeof(memBlock) = %ld\n", sizeof(memBlock));
	//printf("address of ptr1 = %zd\n", (uintptr_t)(ptr1));
	//printf("ptr1 size = %ld\n", ((memBlock*)((void*)ptr1-sizeof(memBlock)))->size);
	//void* ptr2 = mymalloc(50);
	//printf("address of ptr2 = %zd\n", (uintptr_t)(ptr2));
	//printf("ptr2 size = %ld\n", ((memBlock*)(ptr2-sizeof(memBlock)))->size);
	//ptr1 = (int*)myrealloc((void*)ptr1, 100);
	//uintptr_t newAddy = (uintptr_t)ptr1;
	//if (!ptr1) {
	//	printf("ptr1 is null\n");
	//	exit(0);
	//}
	//printf("ptr1 size = %ld\n", ((memBlock*)((void*)ptr1-sizeof(memBlock)))->size);
	//printf("ptr2 size = %ld\n", ((memBlock*)(ptr2-sizeof(memBlock)))->size);
	//if (ogAddy != newAddy) {
	//	for (int i = 0; i < 4; i++)
	//		printf("ptr1[%d] = %d\n", i, ptr1[i]);
	//}
}
