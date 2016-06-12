const std::size_t TAG_SIZE = sizeof(int);
const std::size_t ALLOCATOR_BLOCK_SIZE = 16;



class Allocator
{
private:
	void* Memory;
	int MemorySize;
	Allocator();
	bool IsBlockFree(void*);
	//Works on the assumpion that both blocks are free
	void Coalesce(void* LeftBlock, void* RightBlock);
public:
	Allocator(int Bytes);
	~Allocator();
	void* Allocate(int Bytes);
	//Takes pointer to first usable addres aka address returend by Allocate otherwise UB
	void Deallocate(void*);
	//Takes a pointer to address returned by Allocate
	void PrintTags(void* Pointer);
	void PrintAllocatorTags();
};

template<typename T>
T *IncrementPointer(T *p, int n)
{
	return reinterpret_cast<T *>(reinterpret_cast<char *>(p) + n);
}
