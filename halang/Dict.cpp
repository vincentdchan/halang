
#include <utility>
#include "Dict.h"
#include "type.h"

namespace halang
{

	Dict::hash_type Dict::Hash(Object* obj)
	{
		switch (obj->GetTypeId())
		{
		case TypeId::Number:
		{
			auto num = Number::Cast(obj)->GetValue();
			return std::hash<TNumber>()(num);
			break;
		}
		case TypeId::SmallInt:
		{
			auto si = SmallInt::Cast(obj)->GetValue();
			return std::hash<TSmallInt>()(si);
			break;
		}
		case TypeId::String:
			return String::Cast(obj)->GetHash();
			break;
		default:
			throw std::runtime_error("do hash on wrong type");
		}
	}

	Object* Dict::Get(Object* key)
	{
		auto _hash = Hash(key);
		auto _index = _hash % base_capacity;

		Entry* ptr = base[_index];

		while (ptr != nullptr)
		{
			if (ptr->hash_key == _hash)
				return ptr->value;
			ptr = ptr->next;
		}
		return Context::GetGc()->NewNull();
	}

	void Dict::Set(Object * key, Object * value)
	{
		auto _hash = Hash(key);
		auto _index = _hash & base_capacity;

		Entry* ptr = base[_index];

		if (ptr == nullptr)
		{
			ptr = Context::GetGc()->New<Entry>(_hash, value);
			ptr->next = nullptr;
			base[_index] = ptr;
		}
		else
		{
			while (ptr != nullptr)
			{
				if (ptr->hash_key == _hash)
				{
					ptr->value = value;
					return;
				}
				ptr = ptr->next;
			}
			// not found
			ptr = Context::GetGc()->New<Entry>(_hash, value);
			ptr->next = base[_index];
			base[_index] = ptr;
		}
	}

}