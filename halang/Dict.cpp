#include "Dict.h"

namespace halang
{

	Dict::Dict()
	{}

	Dict::Dict(const Dict& hm) :
		inner_map(hm.inner_map)
	{}

	Dict::Dict(const std::unordered_map<Object, Object>& m) :
		inner_map(m)
	{}

	void Dict::SetValue(Object key, Object value)
	{
		inner_map[key] = value;
	}

	Object Dict::GetValue(Object key)
	{
		return inner_map[key];
	}

}