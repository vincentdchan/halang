#pragma once
#include <cstring>
#include "halang.h"
#include "object.h"

namespace halang
{

	class Dict : public Object
	{

	public:
		friend class GC;

		typedef Object* DataType;
		typedef unsigned int hash_type;
		typedef std::uint32_t size_type;

	protected:

		static const size_type DEFAULT_CAPACITY = 64; // 2 ^ 5
		size_type base_capacity;

		struct Entry
		{
		public:

			Entry(hash_type _hash, DataType _data):
				hash_key(_hash), value(_data), next(nullptr)
			{}

			hash_type hash_key;
			DataType value;
			Entry* next;

		};

		Entry** base;

		void Alloc(size_type c)
		{
			auto base_capacity = c;
			unsigned int len = sizeof(Entry*) * base_capacity;
			auto ptr = reinterpret_cast<Entry**>
				(Context::GetGc()->Alloc(len));
			std::memset(ptr, 0, len);
			base = ptr;
		}

		Dict()
		{
			typeId = TypeId::Dict;
			Alloc(DEFAULT_CAPACITY);
		}

	public:
		
		hash_type Hash(Object*);
		Object* Get(Object*);
		void Set(Object*, Object*);

	};

}
