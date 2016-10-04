#pragma once
#include <cmath>
#include "object.h"
#include "string.h"
#include "GC.h"
#include "svm.h"
#include "Dict.h"

const char* ADD_METHOD = "__add__";
const char* SUB_METHOD = "__sub__";
const char* MUL_METHOD = "__mul__";
const char* DIV_METHOD = "__div__";

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

	class SmallIntPrototype : public Dict
	{
	private:

		static SmallInt* __add__(SmallInt* a, SmallInt* b)
		{
			return Context::GetGc()->NewSmallInt(a->GetValue() + b->GetValue());
		}

		static SmallInt* __sub__(SmallInt* a, SmallInt* b)
		{
			return Context::GetGc()->NewSmallInt(a->GetValue() - b->GetValue());
		}

		static SmallInt* __mul__(SmallInt* a, SmallInt* b)
		{
			return Context::GetGc()->NewSmallInt(a->GetValue() * b->GetValue());
		}

		static SmallInt* __div__(SmallInt* a, SmallInt* b)
		{
			return Context::GetGc()->NewSmallInt(a->GetValue() / b->GetValue());
		}

	protected:

		SmallIntPrototype() : Dict()
		{
			Dict::Set(String::FromCStr(ADD_METHOD), Context::GetGc()->NewNull());
			Dict::Set(String::FromCStr(SUB_METHOD), Context::GetGc()->NewNull());
			Dict::Set(String::FromCStr(MUL_METHOD), Context::GetGc()->NewNull());
			Dict::Set(String::FromCStr(DIV_METHOD), Context::GetGc()->NewNull());
		}

	};

	class NumberPrototype : public Dict
	{
	protected:

		NumberPrototype() : Dict()
		{
			Dict::Set(String::FromCStr(ADD_METHOD), Context::GetGc()->NewNull());
			Dict::Set(String::FromCStr(SUB_METHOD), Context::GetGc()->NewNull());
			Dict::Set(String::FromCStr(MUL_METHOD), Context::GetGc()->NewNull());
			Dict::Set(String::FromCStr(DIV_METHOD), Context::GetGc()->NewNull());
		}

	};

};
