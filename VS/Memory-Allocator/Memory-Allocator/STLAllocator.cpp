#include <limits>
#include <iostream>
#include "MyAllocator.h"

//static MyAllocator MyAlloc(1<<25);

template <class T>
class STLAllocator
{

public:
	// type definitions
	typedef size_t    size_type;
	typedef ptrdiff_t difference_type;
	typedef T*        pointer;
	typedef const T*  const_pointer;
	typedef T&        reference;
	typedef const T&  const_reference;
	typedef T         value_type;
	static MyAllocator MyAlloc;

	// rebind STLAllocator to type U
	template <class U>
	struct rebind
	{
		typedef STLAllocator<U> other;
	};

	// return address of values
	pointer STLAllocator::address(reference value) const
	{
		return &value;
	}
	const_pointer address(const_reference value) const
	{
		return &value;
	}

	STLAllocator()
	{

	}


	STLAllocator(const STLAllocator&)
	{
	}

	template <class U>
	STLAllocator(const STLAllocator<U>&)
	{
	}

	~STLAllocator()
	{
	}

	// return maximum number of elements that can be allocated
	size_type max_size() const
	{
		return MyAlloc.GetMemorySize() / sizeof(T);
	}


	//Use my custom allocator for allocation duh
	pointer allocate(size_type num)
	{
		return(pointer)(MyAlloc.Allocate(num*sizeof(T)));
	}

	void construct(pointer p, const T& value)
	{
		// initialize memory with placement new
		new((void*)p)T(value);
	}

	void destroy(pointer p)
	{
		p->~T();
	}
	//Deallocate using my allocator 
	void deallocate(pointer p, size_type num)
	{
		MyAlloc.Deallocate((void*)p);
	}
};

// return that all specializations of this STLAllocator are interchangeable
template <class T1, class T2>
bool operator== (const STLAllocator<T1>&,
	const STLAllocator<T2>&)
{
	return true;
}
template <class T1, class T2>
bool operator!= (const STLAllocator<T1>&,
	const STLAllocator<T2>&)
{
	return false;
}
template<typename T>
MyAllocator STLAllocator<T>::MyAlloc = 1<<20;