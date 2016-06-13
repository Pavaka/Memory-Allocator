#include <cstdio>
const std::size_t TAG_SIZE = sizeof(int);
const std::size_t ALLOCATOR_BLOCK_SIZE = 16;

class MyAllocator
{
private:
	void* Memory;
	int MemorySize;
	MyAllocator();
	//Works on the assumpion that both blocks are free, 
	//pointers are to the tag begining of them
	void Coalesce(void* LeftBlock, void* RightBlock);
public:
	MyAllocator(int Bytes);
	~MyAllocator();
	void* Allocate(int Bytes);
	//Takes pointer to first usable addres -> address returend by Allocate otherwise undifined behaviour
	void Deallocate(void*);
	//Takes a pointer to address returned by Allocate
	void PrintTags(void* Pointer);
	void PrintAllocatorTags();
	int GetMemorySize();
};

template<typename T>
inline T* IncrementPointer(T* p, int n)
{
	return reinterpret_cast<T *>(reinterpret_cast<char *>(p) + n);
}
