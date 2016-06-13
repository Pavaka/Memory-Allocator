#include <cstdlib>
#include <iostream>
#include <malloc.h>
#include "MyAllocator.h"

MyAllocator::MyAllocator(int Bytes)
{
	if (Bytes > ALLOCATOR_BLOCK_SIZE)
	{
		//Round up to the nearest bytes that are mod ALLOCATOR_BLOCK_SIZE = 0
		Bytes = Bytes +   static_cast<bool>(Bytes%ALLOCATOR_BLOCK_SIZE)*(ALLOCATOR_BLOCK_SIZE - Bytes%ALLOCATOR_BLOCK_SIZE);

		this->Memory = _aligned_malloc(Bytes, ALLOCATOR_BLOCK_SIZE);
		this->MemorySize = Bytes;
		//set boundry tags first and last
		*reinterpret_cast<int*>(reinterpret_cast<char*>(this->Memory) + ALLOCATOR_BLOCK_SIZE - TAG_SIZE) = Bytes - ALLOCATOR_BLOCK_SIZE;
		*reinterpret_cast<int*>(reinterpret_cast<char*>(this->Memory) + Bytes - 2 * TAG_SIZE) = Bytes - ALLOCATOR_BLOCK_SIZE;

		//Set last block  to size zero and used aka 1
		*reinterpret_cast<int*>(reinterpret_cast<char*>(this->Memory) + Bytes - TAG_SIZE) = 1;
		//set the TAG_SIZE before memory to zero and used
		*reinterpret_cast<int*>(reinterpret_cast<char*>(this->Memory) + ALLOCATOR_BLOCK_SIZE - 2*TAG_SIZE) = 1;

	}
	else
	{
		this->Memory =  nullptr;
	}
}

MyAllocator::~MyAllocator()
{
	_aligned_free(this->Memory);
}
//TOFIN
void * MyAllocator::Allocate(int Bytes)
{
	//First Block
	void* CurrentBlockAddress = IncrementPointer(this->Memory, ALLOCATOR_BLOCK_SIZE - TAG_SIZE);
	void* EndOfMemory = IncrementPointer(this->Memory, this->MemorySize - TAG_SIZE);
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

void MyAllocator::Deallocate(void* Pointer)
{
	void* BlockToBeFreed = IncrementPointer(Pointer, -TAG_SIZE);
	int BlockToBeFreedSize = *reinterpret_cast<int*>(BlockToBeFreed) & ~1;
	//Mark the block to be freed as free
	*reinterpret_cast<int*>(BlockToBeFreed) = BlockToBeFreedSize;
	*reinterpret_cast<int*>(IncrementPointer(BlockToBeFreed, BlockToBeFreedSize-TAG_SIZE)) = BlockToBeFreedSize;

	//Check right block if free
	void* RightBlock = IncrementPointer(BlockToBeFreed, BlockToBeFreedSize);
	int RightBlockMask = *reinterpret_cast<int*>(RightBlock) & 1;

	//check left block
	void* LeftBlockRightTag = IncrementPointer(BlockToBeFreed, -TAG_SIZE);
	int LeftBlockMask = *reinterpret_cast<int*>(LeftBlockRightTag) & 1;

	//int LeftBlockSize = *reinterpret_cast<int*>(LeftBlock) & ~1;
	if (!RightBlockMask)
	{
		this->Coalesce(BlockToBeFreed, RightBlock);
	}
	if (!LeftBlockMask)
	{
		//free block so no reason to unmask
		int LeftBlockSize = *reinterpret_cast<int*>(LeftBlockRightTag);
		void* LeftBlock = IncrementPointer(BlockToBeFreed, -LeftBlockSize);
		this->Coalesce(LeftBlock, BlockToBeFreed);
	}


}


void MyAllocator::PrintTags(void* Pointer)
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

void MyAllocator::PrintAllocatorTags()
{
	void* CurrentBlockAddress = IncrementPointer(this->Memory, ALLOCATOR_BLOCK_SIZE - TAG_SIZE);
	std::cout << "---------- PRINT ALLOCATOR --------- \nAllocator Size " << this->MemorySize << std::endl;
	void* EndOfMemory = IncrementPointer(this->Memory, this->MemorySize - TAG_SIZE);
	while (CurrentBlockAddress < EndOfMemory)
	{
		this->PrintTags(IncrementPointer(CurrentBlockAddress, TAG_SIZE));
		CurrentBlockAddress = IncrementPointer(CurrentBlockAddress, (*reinterpret_cast<int*>(CurrentBlockAddress) & ~1));
	}
}

int MyAllocator::GetMemorySize()
{
	return this->MemorySize;
}

void MyAllocator::Coalesce(void * LeftBlock, void * RightBlock)
{
	//Get sizes
	int LeftBlockSize = *reinterpret_cast<int*>(LeftBlock);
	int RightBlockSize = *reinterpret_cast<int*>(RightBlock);
	int NewBlockSize = LeftBlockSize + RightBlockSize;
	//Set both tags
	*reinterpret_cast<int*>(LeftBlock) = NewBlockSize;
	*reinterpret_cast<int*>(IncrementPointer(LeftBlock, NewBlockSize - TAG_SIZE)) = NewBlockSize;
}
