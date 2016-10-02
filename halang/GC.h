#pragma once
#include <cstdlib>
#include <cinttypes>
#include <list>
#include "object.h"
#include "string.h"

namespace halang
{
	class GC
	{
	public:
		typedef std::uint32_t size_type;
		typedef std::int32_t c32;
		typedef std::int16_t c16;
		typedef std::uint32_t uc32;
		typedef std::uint16_t uc16;

		static void SetMark(void*);
		static void ClearMark(void*);

		SimpleString* NewForSimpleString(size_type);

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
		void* Alloc(size_type);
		void Dealloc(void*, size_type);


		std::list<std::pair<void*, size_type> > young_ptrs;
		std::list<std::pair<void*, size_type> > old_ptrs;

		unsigned int young_space_size;
		unsigned int old_space_size;
	protected:

		void MarkYoung();
		void MarkAll();
		void SweepYoung();
		void SweepAll();

	public:

		Object* NewObject();
		Object* NewObjectArray(size_type);
		SmallInt* NewSmallInt(int v = 0);
		Number* NewNumber(double num = 0.0);
		uc32* NewUInt32(uc32 v = 0);
		char* NewChar(unsigned int _size);
		TChar* NewTChar(unsigned int _size);


	};

}
