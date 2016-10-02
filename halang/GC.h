#pragma once
#include <cstdlib>
#include <list>
#include "object.h"
#include "string.h"

namespace halang
{
	class GC
	{
	public:
		static void setMark(void*);
		static void clearMark(void*);

		SimpleString* NewForSimpleString(unsigned int);

		struct MemorySlice
		{

			static MemorySlice* FromPtr(void*);

			unsigned int _generation : 2;
			unsigned int _mark : 2;
			unsigned int _gc_obj_type : 2;
			char that[];

			void* toPtr();

		};


	private:
		void* Alloc(unsigned int);
		void Dealloc(void*);


		std::list<void*> young_pointers;
		std::list<void*> old_pointers;
		unsigned int young_space_size;
		unsigned int old_space_size;
	protected:

		void MarkYoung();
		void MarkAll();
		void SweepYoung();
		void SweepAll();

	public:

		SmallInt* NewSmallInt(int v = 0)
		{
			return new(Alloc(sizeof(SmallInt))) SmallInt(v);
		}

		Number* NewNumber(double num = 0.0)
		{
			return new(Alloc(sizeof(Number))) Number(num);
		}

		char* NewChar(unsigned int _size)
		{
			return reinterpret_cast<char*>(Alloc(_size * sizeof(char)));
		}

		char16_t NewChar16(unsigned int _size)
		{
			return reinterpret_cast<char16_t>(Alloc(_size * sizeof(char)));
		}

	};

	GC::MemorySlice* GC::MemorySlice::FromPtr(void* ptr)
	{
		return reinterpret_cast<MemorySlice*>(
			reinterpret_cast<char*>(ptr) - sizeof(GC::MemorySlice));
	};

	void* GC::MemorySlice::toPtr()
	{
		return (reinterpret_cast<char*>(this) + sizeof(GC::MemorySlice));
	}

	void GC::setMark(void * ptr)
	{
		auto slice = MemorySlice::FromPtr(ptr);
		slice->_mark = 1;
	}

	void GC::clearMark(void * ptr)
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
			return reinterpret_cast<void*>(ptr + sizeof(MemorySlice));
		}
		else
			throw std::runtime_error("memory out.");
	}

	void GC::Dealloc(void* ptr)
	{
		auto ms = MemorySlice::FromPtr(ptr);
		free(ms);
	}

}
