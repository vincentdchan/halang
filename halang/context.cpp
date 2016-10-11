#include "context.h"

namespace halang
{

	GC* Context::GetGC() { return gc; }

	Dict* Context::GetNullPrototype()
	{
		return _null_proto;
	}

	Dict* Context::GetBoolPrototype()
	{
		return _bool_proto;
	}

	Dict* Context::GetSmallIntPrototype()
	{
		return _si_proto;
	}

	Dict* Context::GetNumberPrototype()
	{
		return _num_proto;
	}
		
	GC* Context::gc = nullptr;
	Dict* Context::_null_proto = nullptr;
	Dict* Context::_bool_proto = nullptr;
	Dict* Context::_si_proto = nullptr;
	Dict* Context::_num_proto = nullptr;

}