#include <iostream>
#include "Allocator.h"




int main()
{

	Allocator allocator(1<<11);


	void* All1 = allocator.Allocate(300);
	void* All2 = allocator.Allocate(400);
	void* All3 = allocator.Allocate(100);


	allocator.Deallocate(All2);
	allocator.PrintAllocatorTags();

	allocator.Deallocate(All1);
	allocator.PrintAllocatorTags();

	allocator.Deallocate(All3);
	allocator.PrintAllocatorTags();

	//int* intmem = reinterpret_cast<int*>(MyMemory);
	//for (int i = 0; i < 1<<13; ++i)
	//{
	//	intmem[i] = i;
	//}
	//allocator.PrintAllocatorTags();
	//for (int i = 0; i < 1<<5; ++i)
	//{
	//	std::cout << intmem[i] << std::endl;
	//}



	int x;
	std::cin >> x;
	return 0;
}

//
//void PrintTags(void* Pointer)
//{
//	int MaskedLeftTagValue = *reinterpret_cast<int*>(IncrementPointer(Pointer, -TAG_SIZE));
//	bool IsAllocatedLeft = MaskedLeftTagValue & 1;
//	int UnmaskedLeftTagValue = MaskedLeftTagValue & ~1;
//
//	int MaskedRightTagValue = *reinterpret_cast<int*>(IncrementPointer(Pointer, UnmaskedLeftTagValue - 2* TAG_SIZE));
//	bool IsAllocatedRight = MaskedRightTagValue & 1;
//
//	int UnmaskedRightTagValue = MaskedLeftTagValue & ~1;
//	std::cout << "Pointer = " << Pointer << std::endl;
//	std::cout << "Allocated bits values = " << IsAllocatedLeft << " " << IsAllocatedRight << std::endl;
//	std::cout << "Allocated block size = " << UnmaskedLeftTagValue << " " << UnmaskedRightTagValue << std::endl;
//}