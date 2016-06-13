#include <iostream>
#include "Allocator.h"

int main()
{

	Allocator allocator(2304);


	void* All1 = allocator.Allocate(300);
	void* All2 = allocator.Allocate(400);
	void* All3 = allocator.Allocate(100);
	void* All4 = allocator.Allocate(1050);
	void* All5 = allocator.Allocate(100);
	allocator.PrintAllocatorTags();


	//allocator.Deallocate(All2);
	//allocator.PrintAllocatorTags();

	//allocator.Deallocate(All1);
	//allocator.PrintAllocatorTags();

	//allocator.Deallocate(All3);
	//allocator.PrintAllocatorTags();

	int x;
	std::cin >> x;
	return 0;
}
