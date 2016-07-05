#include <cstdio>
const std::size_t TAG_SIZE = sizeof(int);
const std::size_t ALLOCATOR_BLOCK_SIZE = 16;

class MyAllocator
{
private:
	void* Memory;
	int MemorySize;
	MyAllocator();
	//Coalseces two (used or unused) blocks as a new FREE block sum of them
	//pointers are to the tag begining of them
	void Coalesce(void* LeftBlock, void* RightBlock);
	void* FindSuitableBlock(void* StartingAddress, void* EndAddress, int RequiredSize);
	//Splits a block into two blocks, their availability is based on the original block availability
	//should the returned blocks be marked as used or unused 0 unused 1 used
	std::pair<void*, void*> SplitBlock(void*, int FirstBlockSize, int LeftBlockUsed = 0, int RightBlockUsed = 0);
public:
	MyAllocator(int Bytes);
	~MyAllocator();
	void* Allocate(int Bytes);
	//Takes pointer to first usable addres -> address returend by Allocate otherwise undifined behaviour
	void Deallocate(void*);
	//Takes a pointer to address returned by Allocate
	void PrintTags(void* Pointer);
	void PrintAllocatorTags();
	int GetMemorySize() const;
	void* Reallocate(void*, int);
};

template<typename T>
inline T* IncrementPointer(T* p, int n)
{
	return reinterpret_cast<T *>(reinterpret_cast<char *>(p) + n);
}
