#include "hashmap.h"

namespace halang
{

	HashMap::HashMap()
	{}

	HashMap::HashMap(const HashMap& hm) :
		inner_map(hm.inner_map)
	{}

	HashMap::HashMap(const std::unordered_map<Object, Object>& m) :
		inner_map(m)
	{}

	void HashMap::SetValue(Object key, Object value)
	{
		inner_map[key] = value;
	}

	Object HashMap::GetValue(Object key)
	{
		return inner_map[key];
	}

}