#include "dict.h"

namespace halang
{

	Dict::Dict()
	{
		arrays = new Object[DEFAULT_ARRAY_CAPACITY];
		array_len = DEFAULT_ARRAY_CAPACITY;
	}


}