#include "Dict.h"

namespace halang
{

	void Dict::SetValue(Value key, Value value)
	{
		unordered_map<Value, Value>::operator[](key) = value;
	}

	Value Dict::GetValue(Value key)
	{
		return unordered_map<Value, Value>::at(key);
	}

	bool Dict::Exist(Value key)
	{
		return unordered_map<Value, Value>::find(key) != unordered_map<Value, Value>::end();
	}

}
