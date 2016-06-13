#include <iostream>
#include <vector>
#include <deque>

#include "STLAllocator.cpp"

int main()
{

	MyAllocator allocator(2304);
	void* All1 = allocator.Allocate(300);
	void* All2 = allocator.Allocate(400);
	void* All3 = allocator.Allocate(100);
	//void* All4 = allocator.Allocate(500);
	//void* All5 = allocator.Allocate(100);
	allocator.PrintAllocatorTags();
	allocator.Reallocate(All2, 1000);
	allocator.PrintAllocatorTags();

	//allocator.Deallocate(All2);
	////allocator.PrintAllocatorTags();
	//allocator.Deallocate(All1);
	////allocator.PrintAllocatorTags();
	//allocator.Deallocate(All5);
	////allocator.PrintAllocatorTags();
	//allocator.Deallocate(All3);
	////allocator.PrintAllocatorTags();
	//allocator.Deallocate(All4);
	//allocator.PrintAllocatorTags();


	//void* x = allocator.Allocate(1500);
	//void* y = allocator.Allocate(520);
	//allocator.PrintAllocatorTags();
	//allocator.Deallocate(y);
	//allocator.PrintAllocatorTags();
	//allocator.Deallocate(x);
	//allocator.PrintAllocatorTags();
	//allocator.Allocate(1000);
	//allocator.Allocate(10000);
	//allocator.PrintAllocatorTags();

	//std::deque<int, STLAllocator<int>> z;
	//int sizze = 200;
	//for (int i = 0; i < sizze; ++i)
	//{
	//	z.push_front(i);
	//	z.push_back(i);
	//}
	//for (int i = 0; i < sizze * 2; ++i)
	//{
	//	std::cout << z[i] << " ";
	//}

	//std::vector<int, STLAllocator<int>>  x;
	//
	//for (int i = 0; i < 50000; ++i)
	//{
	//	x.push_back(i);
	//}
	//for (int i = 0; i < 50000; ++i)
	//{
	//	std::cout << x[i] << " ";
	//}

	int zztop;
	std::cin >> zztop;
	return 0;
}
