#pragma once
#include <cstdlib>
#include <cinttypes>
#include <list>
#include <memory>
#include "svm.h"
#include "object.h"
#include "string.h"
#include "context.h"

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

		static bool WhiteMarked(void*);
		static bool GreyMarked(void*);
		static bool BlackMarked(void*);

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


		void* Alloc(size_type);
	private:
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

		template<class _Ty, class... _Types> 
		inline _Ty* New(_Types&&... _Args)
		{	
			auto space = Alloc(sizeof(MemorySlice) + sizeof(_Ty));
			return new(space) _Ty(std::forward<_Types>(_Args)...));
		}

		Object* NewNull();
		Object** NewObjectPointerArray(size_type);
		Instruction* NewInstructionArray(size_type);
		SmallInt* NewSmallInt(int v = 0);
		Number* NewNumber(double num = 0.0);
		uc32* NewUInt32(uc32 v = 0);
		char* NewChar(unsigned int _size);
		TChar* NewTChar(unsigned int _size);

		GC();
		~GC();


	};

}
