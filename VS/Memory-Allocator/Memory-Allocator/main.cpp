#include <iostream>
#include "Allocator.h"




int main()
{

	Allocator allocator(1<<16);

	//void* MyMemory = allocator.Allocate((1<<10) * sizeof(int));

	allocator.Allocate(1 << 10);
	allocator.Allocate(65);
	allocator.PrintAllocatorTags();



	//int* intmem = reinterpret_cast<int*>(MyMemory);
	//for (int i = 0; i < 1<<5; ++i)
	//{
	//	intmem[i] = i;
	//}
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