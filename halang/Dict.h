#pragma once
#include "halang.h"
#include "object.h"
#include "string.h"
#include <utility>
#include <unordered_map>

namespace halang
{
	class Dict : 
		public GCObject
	{
	public:

		friend class GC;
		friend class StackVM;
		typedef unsigned int size_type;

		static const size_type DEFAULT_ENTRY_SIZE = 32;

	protected:

		Dict();
		// Dict(const Dict&);

		struct Entry;

		Entry ** entries;
		size_type _size;

	public:

		Value toValue() override;

		bool TryGetValue(Value key, Value &value);
		bool TryEmplace(Value key, Value value);
		bool TryRemove(Value key);
		bool Exist(Value key);

		void Insert(Value key, Value value);
		Value GetValue(Value key);
		void SetValue(Value key, Value value);

		virtual ~Dict() override;

	};

	struct Dict::Entry
	{
	public:

		Entry() : next(nullptr)
		{}

		Entry(unsigned int _hash, Value _key, Value _value, Entry * _next = nullptr):
			hash(_hash), key(_key), value(_value), next(_next)
		{}

		unsigned int hash;
		Value key;
		Value value;
		Entry * next;
	};

}
