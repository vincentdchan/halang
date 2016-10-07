#pragma once
#include "GC.h"

namespace halang 
{

	class Context final
	{
	public:
		friend class GC;

		inline GC* GetGC() { return gc; }

	private:
		
		static GC* gc;

	};

}

