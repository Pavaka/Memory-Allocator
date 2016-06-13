#include <cstdlib>
#include <iostream>
#include <malloc.h>
#include <cstring>
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
	//Find satisfying block

	//while ( (CurrentBlockAddress < EndOfMemory) && 
	//			( (*reinterpret_cast<int*>(CurrentBlockAddress) & 1) || 
	//				(*reinterpret_cast<int*>(CurrentBlockAddress) < RequiredSize) ) )
	//{
	//	CurrentBlockAddress = IncrementPointer(CurrentBlockAddress, (*reinterpret_cast<int*>(CurrentBlockAddress) & ~1 ));
	//}
	//Use the function
	CurrentBlockAddress = this->FindSuitableBlock(CurrentBlockAddress, EndOfMemory, RequiredSize);

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

void* MyAllocator::Reallocate(void* Pointer, int RequestedBlockSize)
{
	void* OldBlockAddress = IncrementPointer(Pointer, -TAG_SIZE);
	int OldBlockAddressSize = *reinterpret_cast<int*>(OldBlockAddress) & ~1;
	
	int NewBlockSize = RequestedBlockSize + 2 * TAG_SIZE + (ALLOCATOR_BLOCK_SIZE - (RequestedBlockSize + 2 * TAG_SIZE) % 16);
	//If reallocation is to smaller, just shrink it
	if (NewBlockSize == OldBlockAddressSize)
	{
		return Pointer;
	}
	else if (NewBlockSize < OldBlockAddressSize)
	{
		//Make the tags for the new smaller block, to valid size and makred as used
		*reinterpret_cast<int*>(OldBlockAddress) = NewBlockSize | 1;
		*reinterpret_cast<int*>(IncrementPointer(OldBlockAddress, NewBlockSize - TAG_SIZE)) = NewBlockSize | 1;

		int LeftoverBlockSize = OldBlockAddressSize - NewBlockSize;
		void* LeftoverBlcokAddress = IncrementPointer(OldBlockAddress, NewBlockSize);
		//Mark the leftover block to the smaller size
		*reinterpret_cast<int*>(LeftoverBlcokAddress) = LeftoverBlockSize;
		*reinterpret_cast<int*>(IncrementPointer(LeftoverBlcokAddress, LeftoverBlockSize - TAG_SIZE)) = LeftoverBlockSize;


		//See if the right block of the leftover is unusied to coalesce them
		void* BlockRightToLeftoverBlockAddress = IncrementPointer(LeftoverBlcokAddress, LeftoverBlockSize);
		//If the right block is free coalesce them
		if (!(*reinterpret_cast<int*>(BlockRightToLeftoverBlockAddress) & 1))
		{
			Coalesce(LeftoverBlcokAddress, BlockRightToLeftoverBlockAddress);
		}

		return Pointer;
	}
	//TODO EXTEND CURRENT BLOCK
	else if (NewBlockSize > OldBlockAddressSize)
	{
		//If reallocation is not possible return nullptr
		//Thus programmer should not discard the pointer he owned
		//Otherwise he will leak memory
		std::cout << "\n\n ENTER DEBUG \n\n" ;
		//Try to expand with the block after
		void* BlockRightToOldBlockAddress = IncrementPointer(OldBlockAddress, OldBlockAddressSize);
		int BlockRightToOldBlockTag = *reinterpret_cast<int*>(BlockRightToOldBlockAddress);
		int BlockRightToOldBlockMask = BlockRightToOldBlockTag & 1;
		int BlockRightToOldBlockSize = BlockRightToOldBlockTag & ~1;
		//std::cout << BlockRightToOldBlockTag << std::endl;

		//std::cout << OldBlockAddressSize << std::endl;
		//std::cout << BlockRightToOldBlockSize << std::endl;
		//std::cout << NewBlockSize << std::endl;

		////this->PrintTags(IncrementPointer(OldBlockAddress, TAG_SIZE));
		////this->PrintTags(IncrementPointer(BlockRightToOldBlockAddress, TAG_SIZE));
		//std::cout << BlockRightToOldBlockMask << " " << BlockRightToOldBlockSize << std::endl;
		if ((!(bool)BlockRightToOldBlockMask) && (OldBlockAddressSize + BlockRightToOldBlockSize >= NewBlockSize))
		{
			std::cout << "\n\nCASE ONE OENE -------------\n\n";
			////we can extend the curent block
			this->Coalesce(OldBlockAddress, BlockRightToOldBlockAddress);
			this->SplitBlock(OldBlockAddress, NewBlockSize, 1, 0);
			return IncrementPointer(OldBlockAddress, TAG_SIZE);
		}
		else
		{
			std::cout << "\n\nCASE TWO TWO -------------\n\n";

			//Find if there is a large enough free block
			void* StartingSearchAddress = IncrementPointer(this->Memory, ALLOCATOR_BLOCK_SIZE - TAG_SIZE);
			void* EndSearchAddress = IncrementPointer(this->Memory, this->MemorySize - TAG_SIZE);
			void* ReturnedFreeBlockAddress = this->FindSuitableBlock(StartingSearchAddress, EndSearchAddress, NewBlockSize);
			if (ReturnedFreeBlockAddress >= EndSearchAddress)
			{
				//No sufficient free block
				return nullptr;
			}
			int ReturnedFreeBlockSize = *reinterpret_cast<int*>(ReturnedFreeBlockAddress);

			void* NewBlockAddress = ReturnedFreeBlockAddress;
			//Mark the old block as free
			//*reinterpret_cast<int*>(OldBlockAddress) = OldBlockAddressSize;
			//*reinterpret_cast<int*>(IncrementPointer(OldBlockAddress, OldBlockAddressSize - TAG_SIZE)) = OldBlockAddressSize;

			if (ReturnedFreeBlockSize > NewBlockSize)
			{
				//Split and mark them as used and unused
				std::pair<void*, void*> BothBlocks = this->SplitBlock(ReturnedFreeBlockAddress, NewBlockSize, 0, 0);
				void* LeftoverBlockAddress = BothBlocks.second;
				//No point in unmasking unmasked was requsted
				int LeftoverBlockSize = *reinterpret_cast<int*>(LeftoverBlockAddress);
				void* BlockRightToLeftoverBlockAddress = IncrementPointer(LeftoverBlockAddress, LeftoverBlockSize);
				int BlockRightToLeftoverBlockTag = *reinterpret_cast<int*>(BlockRightToLeftoverBlockAddress);
				int BlockRightToLeftoverBlockMask = BlockRightToLeftoverBlockTag & 1;
				if (!BlockRightToLeftoverBlockMask)
				{
					this->Coalesce(LeftoverBlockAddress, BlockRightToLeftoverBlockAddress);
				}

			}
			////Deallocate old block
			//Mark the new block size and used
			*reinterpret_cast<int*>(NewBlockAddress) = NewBlockSize | 1;
			*reinterpret_cast<int*>(IncrementPointer(NewBlockAddress, NewBlockSize - TAG_SIZE)) = NewBlockSize | 1;
			//Copy the content from old address to new
			memcpy(IncrementPointer(NewBlockAddress, TAG_SIZE), IncrementPointer(OldBlockAddress, TAG_SIZE), OldBlockAddressSize - 2 * TAG_SIZE);
			//TOSEE
			this->Deallocate(IncrementPointer(OldBlockAddress, TAG_SIZE));
			return IncrementPointer(NewBlockAddress, TAG_SIZE);
		}

	}


}

void MyAllocator::Coalesce(void* LeftBlock, void* RightBlock)
{
	//Get sizes
	int LeftBlockSize = *reinterpret_cast<int*>(LeftBlock) & ~1;
	int RightBlockSize = *reinterpret_cast<int*>(RightBlock) & ~1;
	int NewBlockSize = LeftBlockSize + RightBlockSize;
	//Set both tags
	*reinterpret_cast<int*>(LeftBlock) = NewBlockSize;
	*reinterpret_cast<int*>(IncrementPointer(LeftBlock, NewBlockSize - TAG_SIZE)) = NewBlockSize;
}

void * MyAllocator::FindSuitableBlock(void * StartingAddress, void * EndAddress, int RequiredSize)
{
	void* CurrentBlockAddress = StartingAddress;
	while ((CurrentBlockAddress < EndAddress) &&
		((*reinterpret_cast<int*>(CurrentBlockAddress) & 1) ||
			(*reinterpret_cast<int*>(CurrentBlockAddress) < RequiredSize)))
	{
		CurrentBlockAddress = IncrementPointer(CurrentBlockAddress, (*reinterpret_cast<int*>(CurrentBlockAddress) & ~1));
	}
	return CurrentBlockAddress;
}

std::pair<void*, void*> MyAllocator::SplitBlock(void* BlockAddress, int FirstBlockSize, int LeftBlockUsed, int RightBlockUsed)
{
	int SecondBlockSize = (*reinterpret_cast<int*>(BlockAddress) & ~1) - FirstBlockSize;
	void* SecondBlockAddress = IncrementPointer(BlockAddress, FirstBlockSize);
	//Set first block tags
	*reinterpret_cast<int*>(BlockAddress) = FirstBlockSize | LeftBlockUsed;
	*reinterpret_cast<int*>(IncrementPointer(BlockAddress, FirstBlockSize - TAG_SIZE)) = FirstBlockSize | LeftBlockUsed;

	*reinterpret_cast<int*>(SecondBlockAddress) = SecondBlockSize | RightBlockUsed;
	*reinterpret_cast<int*>(IncrementPointer(SecondBlockAddress, SecondBlockSize - TAG_SIZE)) = SecondBlockSize | RightBlockUsed;

	return std::make_pair(BlockAddress, SecondBlockAddress);
}
