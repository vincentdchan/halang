#include "Dict.h"

namespace halang
{

	Dict::Dict()
	{
		entries = new Entry*[DEFAULT_ENTRY_SIZE]();
		_size = DEFAULT_ENTRY_SIZE;
	}

	Value Dict::toValue()
	{
		return Value(this, TypeId::Dict);
	}

	bool Dict::TryGetValue(Value key, Value & value)
	{
		auto _hh = std::hash<Value>{}(key);
		auto index = _hh % _size;
		if (entries[index] != nullptr)
		{
			Entry * en = entries[index];
			while (en != nullptr)
			{
				if (en->hash == _hh)
				{
					value = en->value;
					return true;
				}
				en = en->next;
			}
		}
		return false;
	}

	bool Dict::TryEmplace(Value key, Value value)
	{
		auto _hh = std::hash<Value>{}(key);
		auto index = _hh % _size;
		if (entries[index] != nullptr)
		{
			Entry * en = entries[index];
			while (en != nullptr)
			{
				if (en->hash == _hh)
				{
					en->value = value;
					return true;
				}
				en = en->next;
			}
		}
		return false;
	}

	bool Dict::TryRemove(Value key)
	{
		auto _hash = std::hash<Value>{}(key);
		auto index = _hash % _size;
		Entry ** enptr = &entries[index];
		while (*enptr != nullptr)
		{
			if ((*enptr)->hash == _hash)
			{
				auto ptr = *enptr;
				(*enptr)->next = ptr->next;
				delete ptr;
				return true;
			}
			enptr = &((*enptr)->next);
		}
		return false;
	}

	void Dict::Insert(Value key, Value value)
	{
		auto _hash = std::hash<Value>{}(key);
		auto index = _hash % _size;
		auto new_entry = new Entry(_hash, key, value, entries[index]);
		entries[index] = new_entry;
	}

	void Dict::SetValue(Value key, Value value)
	{
		if (!TryEmplace(key, value))
			Insert(key, value);
	}

	Value Dict::GetValue(Value key)
	{
		Value v;
		if (!TryGetValue(key, v))
			return Value();
		return v;
	}

	bool Dict::Exist(Value key)
	{
		auto _hash = std::hash<Value>{}(key);
		auto index = _hash % _size;
		auto en = entries[index];

		while (en != nullptr)
		{
			if (en->key == key)
				return true;
			en = en->next;
		}

		return false;
	}

	Dict::~Dict()
	{
		for (size_type i = 0; i < _size; ++i)
		{
			if (entries[i] != nullptr)
			{
				auto ptr = entries[i];
				while (ptr != nullptr)
				{
					auto tmp = ptr;
					ptr = ptr->next;
					delete tmp;
				}
			}
		}

		delete[] entries;
	}

	void Dict::Mark()
	{
		if (!marked)
		{
			marked = true;
			Entry * enptr;
			for (size_type i = 0; i < _size; ++i)
			{
				enptr = entries[i];
				while (enptr != nullptr)
				{
					if (enptr->key.isGCObject())
						enptr->key.value.gc->Mark();
					if (enptr->value.isGCObject())
						enptr->value.value.gc->Mark();
					enptr = enptr->next;
				}
			}
		}
	}

}
