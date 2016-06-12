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

		//Set last block  to size zero and used aka 1
		*reinterpret_cast<int*>(reinterpret_cast<char*>(this->Memory) + Bytes - TAG_SIZE) = 1;
	}
	else
	{
		this->Memory =  nullptr;
	}
}

Allocator::~Allocator()
{
	_aligned_free(this->Memory);
}
//TOFIN
void * Allocator::Allocate(int Bytes)
{
	//First Block
	void* CurrentBlockAddress = IncrementPointer(this->Memory, ALLOCATOR_BLOCK_SIZE - TAG_SIZE);
	void* EndOfMemory = IncrementPointer(this->Memory, this->MemorySize);
	//Required block size
	int RequiredSize = Bytes + 2 * TAG_SIZE + (ALLOCATOR_BLOCK_SIZE - (Bytes + 2 * TAG_SIZE) % 16);
	//std::cout << CurrentBlockAddress << std::endl;
	//std::cout << EndOfMemory << std::endl;
	//std::cout << "Required size = " << RequiredSize << std::endl;
	//std::cout << ( int)EndOfMemory - (int)this->Memory << std::endl;
	////Find satisfying block
	while ( (CurrentBlockAddress < EndOfMemory) && 
				( (*reinterpret_cast<int*>(CurrentBlockAddress) & 1) || 
					(*reinterpret_cast<int*>(CurrentBlockAddress) < RequiredSize) ) )
	{
		CurrentBlockAddress = IncrementPointer(CurrentBlockAddress, (*reinterpret_cast<int*>(CurrentBlockAddress) & ~1 ));
	}

	int CurrentBlockSize = *reinterpret_cast<int*>(CurrentBlockAddress) &  ~1;

	if (CurrentBlockAddress >= EndOfMemory)
	{
		return nullptr;
	}
	//If block size is bigger than the required size e.g there is space left
	else if (CurrentBlockSize > RequiredSize)
	{
		int NewUnallocatedBlockSize = CurrentBlockSize - RequiredSize;
		void* NewUnallocatedBlockAddress = IncrementPointer(CurrentBlockAddress, RequiredSize);
		//set new tags of smaller block
		*reinterpret_cast<int*>(NewUnallocatedBlockAddress) = NewUnallocatedBlockSize;
		*reinterpret_cast<int*>(IncrementPointer(NewUnallocatedBlockAddress, NewUnallocatedBlockSize - TAG_SIZE)) = NewUnallocatedBlockSize;

	}

	//Mask last bit as taken for the returned block
	*reinterpret_cast<int*>(CurrentBlockAddress) = RequiredSize | 1;
	*reinterpret_cast<int*>(IncrementPointer(CurrentBlockAddress, RequiredSize - TAG_SIZE)) = RequiredSize | 1;
	//return the first address after tag
	return IncrementPointer(CurrentBlockAddress, TAG_SIZE);
	
}

void Allocator::Deallocate(void *)
{

}


void Allocator::PrintTags(void* Pointer)
{
	int MaskedLeftTagValue = *reinterpret_cast<int*>(IncrementPointer(Pointer, -TAG_SIZE));
	bool IsAllocatedLeft = MaskedLeftTagValue & 1;
	int UnmaskedLeftTagValue = MaskedLeftTagValue & ~1;

	int MaskedRightTagValue = *reinterpret_cast<int*>(IncrementPointer(Pointer, UnmaskedLeftTagValue - 2 * TAG_SIZE));
	bool IsAllocatedRight = MaskedRightTagValue & 1;

	int UnmaskedRightTagValue = MaskedLeftTagValue & ~1;
	std::cout << "-- Tag Print -- " << std::endl;
	std::cout << "Pointer = " << Pointer << std::endl;
	std::cout << "Allocated bits values = " << IsAllocatedLeft << " " << IsAllocatedRight << std::endl;
	std::cout << "Allocated block size = " << UnmaskedLeftTagValue << " " << UnmaskedRightTagValue << std::endl;
}

void Allocator::PrintAllocatorTags()
{
	void* CurrentBlockAddress = IncrementPointer(this->Memory, ALLOCATOR_BLOCK_SIZE - TAG_SIZE);
	std::cout << "Allocator Size " << this->MemorySize << std::endl;
	void* EndOfMemory = IncrementPointer(this->Memory, this->MemorySize);
	while (CurrentBlockAddress < IncrementPointer(EndOfMemory, -TAG_SIZE))
	{
		std::cout << "allocator tags " << CurrentBlockAddress << std::endl;
		this->PrintTags(IncrementPointer(CurrentBlockAddress, TAG_SIZE));
		CurrentBlockAddress = IncrementPointer(CurrentBlockAddress, (*reinterpret_cast<int*>(CurrentBlockAddress) & ~1));
	}
}
