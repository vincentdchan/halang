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

	protected:

		Dict() {}

	public:

		Value toValue() override;
		Value GetValue(Value key);
		void SetValue(Value key, Value value);
		bool Exist(Value key);

	};
}
