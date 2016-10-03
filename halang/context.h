#pragma once
#include "object.h"

namespace halang
{

	// All static
	class Context
	{
	public:

		static GC* GetGc() { return gc; }
		static StackVM* GetVM() { return vm; }

		friend class StackVM;
		friend class GC;
		
	private:

		static GC* gc;
		static StackVM* vm;

	};

};
