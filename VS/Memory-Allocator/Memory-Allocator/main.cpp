#include <iostream>
#include "Allocator.h"
int main()
{
	std::cout << sizeof(int);

	Allocator allocator(1<<10);
	void* MyMemory = allocator.Allocate(128 * sizeof(int));
	std::cout << MyMemory << std::endl;
	int* intmem = reinterpret_cast<int*>(MyMemory);
	for (int i = 0; i < 128; ++i)
	{
		intmem[i] = i;
	}
	for (int i = 0; i < 128; ++i)
	{
		std::cout << intmem[i] << std::endl;
	}



	int x;
	std::cout << sizeof(int);
	std::cin >> x;
	return 0;
}