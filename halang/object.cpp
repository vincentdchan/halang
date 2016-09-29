#include "stdafx.h"
#include "object.h"
#include "string.h"
#include "svm.h"


namespace halang
{
	static IString PROTOTYPE_NAME("__prototype__");
	static IString EXTEND_NAME("__extend__");

	Object::Object(const Object& _obj)
	{
		type = _obj.type;
		if (_obj.type == TYPE::STRING)
		{
			value.str = new IString(*_obj.value.str);
		}
		else
		{
			value = _obj.value;
		}
	}

	Object::Object(Object&& _obj)
	{
		type = _obj.type;
		if (_obj.type == TYPE::STRING)
		{
			value.str = new IString(*_obj.value.str);
		}
		else
		{
			value = _obj.value;
		}
	}

	Object& Object::operator=(const Object& _obj)
	{
		type = _obj.type;
		if (_obj.type == TYPE::STRING)
		{
			value.str = new IString(*_obj.value.str);
		}
		else
		{
			value = _obj.value;
		}
		return *this;
	}

	Object& Object::operator=(Object&& _obj)
	{
		type = _obj.type;
		if (_obj.type == TYPE::STRING)
		{
			value.str = new IString(*_obj.value.str);
		}
		else
		{
			value = _obj.value;
		}
		return *this;
	}

	TNumber toNumber(Object _obj)
	{
		if (_obj.isNumber())
			return _obj.value.number;
		else if (_obj.isSmallInt())
			return static_cast<TNumber>(_obj.value.si);
		else
			return 0.0;
	}

	Map::Map()
	{}

	Map::Map(const Map& hm) :
		inner_map(hm.inner_map)
	{}

	Map::Map(const std::unordered_map<Object, Object>& m) :
		inner_map(m)
	{}

	void Map::SetValue(Object key, Object value)
	{
		inner_map[key] = value;
	}

	bool Map::Exists(Object key)
	{
		return inner_map.find(key) == inner_map.end();
	}

	Object Map::GetValue(Object key)
	{
		auto i = inner_map.find(key);
		if (i == inner_map.end())
			throw std::runtime_error("key doesn't exisit");
		return i->second;
	}

	GeneralObject::GeneralObject()
	{ 
		SetValue(Object(PROTOTYPE_NAME), Object());
	}

	GeneralObject::GeneralObject(const GeneralObject& _obj) :
		Map(_obj)
	{}

	Map* GeneralObject::GetPrototype()
	{
		auto obj = GetValue(Object(PROTOTYPE_NAME));
		if (obj.isNul())
			return nullptr;
		else
			return reinterpret_cast<Map*>(obj.value.gc);
	}

	Array* Array::Concat(StackVM *vm, Array* a, Array* b)
	{
		auto new_arr =vm->make_gcobject<Array>(*a);
		new_arr->insert(new_arr->end(), 
			b->begin(), b->end());
		return new_arr;
	}

	Array* Array::Slice(StackVM* vm, Array* arr,
		std::size_t begin, std::size_t end)
	{
		auto new_arr = vm->make_gcobject<Array>();

		for (auto i = begin; i != end; ++i)
			new_arr->push_back((*arr)[i]);

		return new_arr;
	}

	void Array::Push(Object obj)
	{
		this->push_back(obj);
	}

	void Array::SetValue(std::size_t index, Object obj)
	{
		if (index >= GetLength())
			throw std::out_of_range("Index out of range");
		this->operator[](index) = obj;
	}

	void Array::RemoveAt(std::size_t index)
	{
		if (index >= GetLength())
			throw std::out_of_range("Index out of range");

		auto it = this->begin();
		std::advance(it, index);
		this->erase(it);
	}

	Object Array::Pop()
	{
		auto obj = this->back();
		this->pop_back();
		return obj;
	}

	void GeneralObject::SetPrototype(Map* pt)
	{
		auto key = Object(PROTOTYPE_NAME);
		if (pt == nullptr)
			SetValue(key, Object());
		else
			SetValue(key, Object(pt, Object::TYPE::MAP));
	}

	std::ostream& operator<<(std::ostream& _ost, Object _obj)
	{
		switch (_obj.type)
		{
		case Object::TYPE::BOOL:
			return _ost << (_obj.value.bl ? STRUE : SFALSE);
		case Object::TYPE::GC:
			return _ost << "<Object>";
		case Object::TYPE::NUL:
			return _ost << "null";
		case Object::TYPE::NUMBER:
			return _ost << _obj.value.number;
		case Object::TYPE::SMALL_INT:
			return _ost << _obj.value.si;
		case Object::TYPE::STRING:
			return _ost << *_obj.value.str;
		}
		return _ost;
	}
};

