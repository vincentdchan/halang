#include "stdafx.h"
#include "string_pool.h"

namespace halang
{

	IString StringPool::getString(IString _Str)
	{
		auto _len = _Str.getLength();
		auto _hash = _Str.getHash();
		if (_len > RESERVE_MAX_LEN)
			return _Str;
		else
		{
			if (_map.find(_hash) == _map.end())
			{
				_map[_hash] = _Str;
				return _Str;
			}
			else
			{
				return _map[_hash];
			}
		}
	}

};