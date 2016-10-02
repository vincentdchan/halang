#pragma once
#include <cinttypes>

namespace halang
{

	enum class TypeId : uint32_t
	{
		Null = 0,
		Bool,
		SmallInt,
		Number,
		GCObject,
		UpValue,
		String,
		Map,
		Dict,
		CodePack,
		Array,
		Function,
	};

	enum class TypeFlag : uint32_t
	{
		Normal,
		ReadOnly,
	};

}
