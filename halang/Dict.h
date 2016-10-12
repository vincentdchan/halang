#pragma once
#include "halang.h"
#include "object.h"
#include "string.h"
#include <utility>
#include <unordered_map>

namespace halang
{
	class Dict : 
		public GCObject, private std::unordered_map<Value, Value>
	{
	public:

		friend class GC;
		friend class StackVM;
		typedef unsigned int size_type;

		static const size_type DEFAULT_ENTRY_SIZE = 32;

	protected:

		Dict() :
			std::unordered_map<Value,Value>(20)
		{
		}

		struct Entry;

	public:

		Value toValue() override;
		Value GetValue(Value key);
		void SetValue(Value key, Value value);
		bool Exist(Value key);

	};

	struct Dict::Entry
	{
	public:
		unsigned int hash;
		Value Key;
		Value Value;
	};

}
