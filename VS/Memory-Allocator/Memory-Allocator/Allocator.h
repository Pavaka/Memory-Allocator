const std::size_t TAG_SIZE = sizeof(int);
const std::size_t ALLOCATOR_BLOCK_SIZE = 16;
class Allocator
{
private:
	void* Memory;
	int MemorySize;
	Allocator();
	bool IsBlockFree(void*);
public:
	Allocator(int Bytes);
	~Allocator();
	void* Allocate(int Bytes);
	void Deallocate(void*);
};

template<typename T>
T *IncrementPointer(T *p, int n)
{
	return reinterpret_cast<T *>(reinterpret_cast<char *>(p) + n);
}
