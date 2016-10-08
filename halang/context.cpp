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

}