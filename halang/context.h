#pragma once
#include "GC.h"
#include "svm.h"

namespace halang
{

	// All static
	class Context
	{
	public:

		static GC* GetGc() { return gc; }
		static StackVM* GetVM() { return vm; }
		
	private:
		static GC* gc;
		static StackVM* vm;
	};

};
