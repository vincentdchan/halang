#pragma once
#include "GC.h"

namespace halang 
{

	class Function;
	class FunctionArgs;

	class Context final
	{
	public:

		friend class GC;
		friend class StackVM;

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

		static void InitializeDefaultPrototype();

		static Value _null_str_(Value self, FunctionArgs& args);

		static Value _si_add_(Value self, FunctionArgs& args);
		static Value _si_sub_(Value self, FunctionArgs& args);
		static Value _si_mul_(Value self, FunctionArgs& args);
		static Value _si_div_(Value self, FunctionArgs& args);
		static Value _si_mod_(Value self, FunctionArgs& args);
		static Value _si_eq_(Value self, FunctionArgs& args);
		static Value _si_gt_(Value self, FunctionArgs& args);
		static Value _si_lt_(Value self, FunctionArgs& args);
		static Value _si_gteq_(Value self, FunctionArgs& args);
		static Value _si_lteq_(Value self, FunctionArgs& args);
		static Value _si_str_(Value self, FunctionArgs& args);

		static Value _num_add_(Value self, FunctionArgs& args);
		static Value _num_sub_(Value self, FunctionArgs& args);
		static Value _num_mul_(Value self, FunctionArgs& args);
		static Value _num_div_(Value self, FunctionArgs& args);
		static Value _num_eq_(Value self, FunctionArgs& args);
		static Value _num_gt_(Value self, FunctionArgs& args);
		static Value _num_lt_(Value self, FunctionArgs& args);
		static Value _num_gteq_(Value self, FunctionArgs& args);
		static Value _num_lteq_(Value self, FunctionArgs& args);
		static Value _num_str_(Value self, FunctionArgs& args);

	};

}

