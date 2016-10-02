
#include <cstring>
#include "GC.h"

namespace halang
{

	// **********************************************
	// New Something
	// **********************************************
	// **********************************************

	Object* GC::NewObject()
	{
		return NewObjectArray(1);
	}

	Object* GC::NewObjectArray(size_type _size)
	{
		return reinterpret_cast<Object*>(Alloc(_size * sizeof(Object*)));
	}

	SmallInt* GC::NewSmallInt(int v)
	{
		return new(Alloc(sizeof(SmallInt))) SmallInt(v);
	}

	Number* GC::NewNumber(double num)
	{
		return new(Alloc(sizeof(Number))) Number(num);
	}

	GC::uc32* GC::NewUInt32(uc32 v)
	{
		return reinterpret_cast<uc32*>(Alloc(sizeof(uc32)));
	}

	char* GC::NewChar(unsigned int _size)
	{
		return reinterpret_cast<char*>(Alloc(_size * sizeof(char)));
	}

	TChar* GC::NewTChar(unsigned int _size)
	{
		auto ptr = reinterpret_cast<TChar*>(Alloc(_size * sizeof(TChar)));
		memset(ptr, 0, _size * sizeof(TChar));
		return ptr;
	}

	GC::MemorySlice* GC::MemorySlice::FromPtr(void* ptr)
	{
		return reinterpret_cast<MemorySlice*>(
			reinterpret_cast<char*>(ptr) - sizeof(GC::MemorySlice));
	};

	void* GC::MemorySlice::toPtr()
	{
		return (reinterpret_cast<char*>(this) + sizeof(GC::MemorySlice));
	}

	void GC::SetMark(void * ptr)
	{
		auto slice = MemorySlice::FromPtr(ptr);
		slice->_mark = 1;
	}

	void GC::ClearMark(void * ptr)
	{
		auto slice = MemorySlice::FromPtr(ptr);
		slice->_mark = 0;
	}

	void* GC::Alloc(unsigned int size)
	{
		auto total_size = sizeof(MemorySlice) + size;

		
		auto ptr = reinterpret_cast<MemorySlice*>(malloc(sizeof(MemorySlice) + size));
		ptr->_mark = 0;
		ptr->_generation = 0;
		ptr->_gc_obj_type = 0;

		if (ptr != nullptr)
		{
			young_space_size += total_size;
			young_ptrs.push_back(std::make_pair(ptr, total_size));
			return reinterpret_cast<void*>(ptr + sizeof(MemorySlice));
		}
		else
			throw std::runtime_error("memory out.");
	}

	void GC::Dealloc(void* ptr, GC::size_type size)
	{
		auto ms = MemorySlice::FromPtr(ptr);
		free(ms);
		young_space_size -= size;
	}

}