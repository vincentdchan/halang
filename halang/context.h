#pragma once
#include "GC.h"

namespace halang 
{

	class Context final
	{
	public:

		friend class GC;

		static GC* GetGC();

		static Dict* GetNullPrototype();
		static Dict* GetBoolPrototype();
		static Dict* GetSmallIntPrototype();
		static Dict* GetNumberPrototype();

	private:
		
		static GC* gc;

		static Dict* _null_proto;
		static Dict* _bool_proto;
		static Dict* _si_proto;
		static Dict* _num_proto;

	};

}

