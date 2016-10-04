
#include <cstring>
#include "GC.h"

namespace halang
{

	// **********************************************
	// New Something
	// **********************************************
	// **********************************************

	Object** GC::NewObjectPointerArray(size_type _size)
	{
		size_type total_size = _size * sizeof(Object*);
		auto ptr = reinterpret_cast<Object**>(Alloc(total_size));
		memset(ptr, 0, total_size);
		return ptr;
	}

	Instruction* GC::NewInstructionArray(size_type _size)
	{
		size_type total_size = _size * sizeof(Instruction);
		auto ptr = reinterpret_cast<Instruction*>(Alloc(total_size));
		memset(ptr, 0, total_size);
		return ptr;
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

	void GC::SweepYoung()
	{

		for (auto i = old_ptrs.begin();
			i != old_ptrs.end(); )
		{
			if (WhiteMarked(i->first))
			{
				Dealloc(i->first, i->second);
				i = old_ptrs.erase(i);
			}
			else
				i++;
		}

	}

	void GC::SweepAll()
	{
		SweepYoung();
		for (auto i = old_ptrs.begin();
			i != old_ptrs.end(); )
		{
			if (WhiteMarked(i->first))
			{
				Dealloc(i->first, i->second);
				i = old_ptrs.erase(i);
			}
			else
				i++;
		}
	}

	void* GC::Alloc(size_type size)
	{
		auto total_size = sizeof(MemorySlice) + size;

		
		auto ptr = reinterpret_cast<MemorySlice*>(std::malloc(sizeof(MemorySlice) + size));

		if (ptr != nullptr)
		{
			ptr->_mark = 0;
			ptr->_generation = 0;
			ptr->_gc_obj_type = 0;

			young_space_size += total_size;
			young_ptrs.push_back(std::make_pair(ptr, total_size));
			return reinterpret_cast<void*>(ptr->that);
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

	GC::GC()
	{
		Context::gc = this;
	}


	GC::~GC()
	{
		for (auto i = young_ptrs.begin(); i != young_ptrs.end(); ++i)
			Dealloc(i->first, i->second);
		for (auto i = old_ptrs.begin(); i != young_ptrs.end(); ++i)
			Dealloc(i->first, i->second);
		Context::gc = nullptr;
	}

}