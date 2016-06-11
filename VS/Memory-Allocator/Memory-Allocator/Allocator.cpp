#include <cstdlib>
#include <iostream>
#include <malloc.h>
#include "Allocator.h"
bool Allocator::IsBlockFree(void* Block)
{

	return false;
}

Allocator::Allocator(int Bytes)
{
	if (Bytes > ALLOCATOR_BLOCK_SIZE  && Bytes % ALLOCATOR_BLOCK_SIZE == 0)
	{
		this->Memory = _aligned_malloc(Bytes, ALLOCATOR_BLOCK_SIZE);
		this->MemorySize = Bytes;
		//set boundry tags first and last
		*reinterpret_cast<int*>(reinterpret_cast<char*>(this->Memory) + ALLOCATOR_BLOCK_SIZE - TAG_SIZE) = Bytes - ALLOCATOR_BLOCK_SIZE;
		*reinterpret_cast<int*>(reinterpret_cast<char*>(this->Memory) + Bytes - 2 * TAG_SIZE) = Bytes - ALLOCATOR_BLOCK_SIZE;

		//Set it to size zero and used aka 1
		*reinterpret_cast<int*>(reinterpret_cast<char*>(this->Memory) + Bytes - TAG_SIZE) = 1;
		std::cout << this->Memory << std::endl;
	}
	else
	{
		this->Memory =  nullptr;
	}
}

Allocator::~Allocator()
{
	free(this->Memory);
}

void * Allocator::Allocate(int Bytes)
{
	//void* CurrentBlockAddress = reinterpret_cast<char*>(this->Memory) + ALLOCATOR_BLOCK_SIZE - TAG_SIZE;
	//char* EndOfMemory = reinterpret_cast<char*>(this->Memory) + this->MemorySize;

	void* CurrentBlockAddress = IncrementPointer(this->Memory, ALLOCATOR_BLOCK_SIZE - TAG_SIZE);
	void* EndOfMemory = IncrementPointer(this->Memory, this->MemorySize);
	int RequiredSize = Bytes + 2 * TAG_SIZE + (ALLOCATOR_BLOCK_SIZE - (Bytes + 2 * TAG_SIZE) % 16);
	std::cout << CurrentBlockAddress << std::endl;
	std::cout << EndOfMemory << std::endl;
	std::cout << RequiredSize << std::endl;
	std::cout << ( int)EndOfMemory - ( int)this->Memory << std::endl;
	////Find satisfying block
	while ( (CurrentBlockAddress < EndOfMemory) && 
				( (*reinterpret_cast<int*>(CurrentBlockAddress) & 1) || 
					(*reinterpret_cast<int*>(CurrentBlockAddress) < RequiredSize) ) )
	{
		CurrentBlockAddress = IncrementPointer(CurrentBlockAddress, *reinterpret_cast<int*>(CurrentBlockAddress));
	}

	if (CurrentBlockAddress >= EndOfMemory)
	{
		return nullptr;
	}
	else if (*reinterpret_cast<int*>(CurrentBlockAddress) == RequiredSize)
	{
		return reinterpret_cast<void*>(CurrentBlockAddress);
	}
	/*return nullptr;*/
	
}

void Allocator::Deallocate(void *)
{
}
