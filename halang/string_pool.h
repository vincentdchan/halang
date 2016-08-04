#pragma once
#include <unordered_map>
#include <cstring>
#include "object.h"
#include "string.h"

namespace halang
{

	class StringPool
	{
	public:
		static const unsigned int RESERVE_MAX_LEN = 50;
		StringPool() {}
		// IString getString(const char* _Str);
		IString getString(IString _Str);
	private:
		std::unordered_map<unsigned int, IString> _map;
	};

};
