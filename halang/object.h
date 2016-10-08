#pragma once
#include <unordered_map>
#include <cmath>
#include <ostream>
#include <string>
#include <exception>
#include <vector>
#include "halang.h"
#include "string.h"

namespace halang
{
	/*
	Value:
		- null
		- bool
		- small int
		- double

		- GC Object
			- String
			- CodePack
			- Map // an hash map
				- General Object
					- Class				// to generate general object
					- Array
					- HashMap
					- Function
					- StringBuilder

	*/

#define OBJ_LIST(V) \
	V(NUL) \
	V(GC) \
	V(UPVALUE) \
	V(STRING) \
	V(MAP) \
	V(ARRAY) \
	V(CODE_PACK) \
	V(FUNCTION) \
	V(SMALL_INT) \
	V(NUMBER) \
	V(BOOL)

	class Object;
	class String;
	class Array;
	class Dict;

	class Isolate;

	struct Value;

	class GCObject
	{
	public:

		friend class GC;

		Dict* GetPrototype();

	protected:

		GCObject* next;
		bool marked;

		virtual Value GetValue() { return Value(); };
		virtual void Mark() {}
		virtual ~GCObject() {}
	};

	enum class TypeId {
		Null,
		Bool,
		SmallInt,
		Number,
		GCObject,
		String,
	};

	union _Value
	{
		GCObject* gc;
		String *str;
		TSmallInt si;		// small int
		TNumber number;
		TBool bl;
	};

	struct Value 
	{
	public:

		_Value value;
		TypeId type;

		Dict* GetPrototype();

		explicit Value() : type(TypeId::Null) {
			value.gc = nullptr;
		}

		explicit Value(TSmallInt i) : type(TypeId::SmallInt) {
			value.si = i;
		}

		explicit Value(TNumber n) : type(TypeId::Number) {
			value.number = n;
		}

		explicit Value(bool n) : type(TypeId::Bool) {
			value.bl = n;
		}

		explicit Value(GCObject* gc, TypeId t) : type(t) {
			value.gc = gc;
		}

		inline bool isNull() const { return type == TypeId::Null; }
		inline bool isBool() const { return type == TypeId::Bool; }
		inline bool isSmallInt() const { return type == TypeId::SmallInt; }
		inline bool isNumber() const { return type == TypeId::Number; }

	};

	/// <summary>
	/// A map is a base data structure in halang
	/// it's used inner to be the base of the GeneralObject
	/// </summary>
	class Map : public GCObject
	{
	public:

		friend class GC;

	protected:

		Map();
		Map(const Map&);
		Map(const std::unordered_map<Object, Object>&);

		void SetValue(Object, Object);
		Object GetValue(Object);
		bool Exists(Object);

	private:

		std::unordered_map<Object, Object> inner_map;

	};

	/// <summary>
	/// Generalized Object must have a dict typed prototype
	/// but generialed obect itself may not be a dict.
	/// </summary>
	class IGeneralObject
	{
		virtual Map* GetPrototype() = 0;
		virtual void SetPrototype(Map*) = 0;
	};

	/// <summary>
	/// A general obect is base on the Map
	/// </summary>
	class GeneralObject :
		public IGeneralObject, public Map
	{
	public:
		

		GeneralObject();
		GeneralObject(const GeneralObject& _obj);

		virtual Map* GetPrototype() override;
		virtual void SetPrototype(Map* pt) override;

	};

	class StackVM;

	/// <summary>
	/// An array support bellow operation;
	///		- GetLength
	///		- At
	///		- Push
	///		- SetValue
	///		- GetValue
	///		- Pop
	///		- RemoveAt
	///	
	///	This version of Array is implemented by the std::vector
	/// </summary>
	class Array : 
		public IGeneralObject, public Map, private std::vector<Object>
	{
	public:

		static Array* Concat(StackVM* vm, Array* a, Array* b);
		static Array* Slice(StackVM* vm, Array*, 
			std::size_t begin, std::size_t end);

		Array():
			_proto(nullptr)
		{}

		inline std::size_t GetLength() { return this->size(); }
		inline Object At(std::size_t index) const { return std::vector<Object>::operator[](index); }

		Object operator[](std::size_t index) const
		{
			return std::vector<Object>::operator[](index);
		}

		void Push(Object obj);
		void SetValue(std::size_t index, Object);
		Object Pop();
		void RemoveAt(std::size_t);

		virtual Map* GetPrototype() override { return _proto; }
		virtual void SetPrototype(Map* pt) override { _proto = reinterpret_cast<GeneralObject*>(pt); }

	private:

		GeneralObject* _proto;

	};

	template<class _ObjectType>
	class Local : std::false_type
	{
		Object toObject();
		Isolate* isolte;
	};

	template<>
	class Local<Object> final : public Object
	{ 
	public:
		Local(): Object()
		{}
		Local(const Object& obj):
			Object(obj)
		{}

		inline Object toObject() { return *this; }

		inline Local<TNumber> asNumber();
		inline Local<TSmallInt> asSmallInt();
		inline Local<Array> asArray();
		inline Local<Map> asMap();
	private:
		Isolate* isolate;
	};

	template<>
	class Local<TNumber>
	{
	private:
		TNumber number;
	public:

		Local(TNumber _num) :
			number(_num)
		{ }

		Local(Object _obj) 
		{
			if (_obj.type != Object::TYPE::NUMBER)
				throw std::runtime_error("local type create error.");
			number = _obj.value.number;
		}

		inline Local<TNumber> operator+(Local<TNumber> obj) const
		{
			return Local<TNumber>(number + obj.number);
		}

		inline Local<TNumber> operator-(Local<TNumber> obj) const
		{
			return Local<TNumber>(number - obj.number);
		}

		inline Local<TNumber> operator*(Local<TNumber> obj) const
		{
			return Local<TNumber>(number * obj.number);
		}

		inline Local<TNumber> operator/(Local<TNumber> obj) const
		{
			return Local<TNumber>(number / obj.number);
		}

		inline Local<TNumber> operator=(const Local<TNumber>& obj)
		{
			number = obj.number;
		}

		inline bool operator==(const Local<TNumber>& obj) const
		{
			number == obj.number;
		}

		inline Object toObject()
		{
			return Object(number);
		}

	};

	template<>
	class Local<TSmallInt>
	{
	private:

		TSmallInt si;

	public:

		Local(TSmallInt num) :
			si(num)
		{ }

		Local(Object _obj) 
		{
			if (_obj.type != Object::TYPE::SMALL_INT)
				throw std::runtime_error("local type create error.");
			si = _obj.value.si;
		}

		inline Object toObject()
		{
			return Object(si);
		}

		inline Local<TSmallInt> operator+(Local<TSmallInt> obj) const
		{
			return Local<TSmallInt>(si + obj.si);
		}

		inline Local<TSmallInt> operator-(Local<TSmallInt> obj) const
		{
			return Local<TSmallInt>(si - obj.si);
		}

		inline Local<TSmallInt> operator*(Local<TSmallInt> obj) const
		{
			return Local<TSmallInt>(si * obj.si);
		}

		inline Local<TSmallInt> operator/(Local<TSmallInt> obj) const
		{
			return Local<TSmallInt>(si / obj.si);
		}

		inline Local<TSmallInt> operator=(const Local<TSmallInt>& obj)
		{
			si = obj.si;
		}

		inline bool operator==(const Local<TSmallInt>& obj) const
		{
			si == obj.si;
		}

	};

	template<>
	class Local<Array>
	{
	private:

		Array* _array;
		Isolate* isolate;

	public:

		Local(Isolate* iso, Array* arr): 
			isolate(iso), _array(arr)
		{}
		Local(Isolate* iso, Object _obj) :
			isolate(iso)
		{
			if (!_obj.isArray())
				throw std::runtime_error("cannot bind local array to object");
			_array = reinterpret_cast<Array*>(_obj.value.gc);
		}
		Local(const Local<Array>& _arr):
			_array(_arr._array), isolate(_arr.isolate)
		{}

		inline Object toObject()
		{
			return Object(_array, Object::TYPE::ARRAY);
		}

		Local<Object> operator[](std::size_t index) const
		{
			return Local<Object>((*_array)[index]);
		}

		template<typename _Ty>
		inline void Push(Local<_Ty> obj)
		{
			this->_array->Push(obj.toObject());
		}

		inline Local<Object> Pop()
		{
			return Local<Object>(_array->Pop());
		}

	};

	template<>
	class Local<Map>
	{
	private:
		Map *map;
		Isolate *isolate;
	public:
		Local(Isolate* iso, Map* _map):
			isolate(iso), map(_map)
		{}

		Object toObject()
		{
			return Object(map, Object::TYPE::MAP);
		}

	};

	Local<TNumber> Local<Object>::asNumber()
	{
		return Local<TNumber>(this->value.number);
	}

	Local<TSmallInt> Local<Object>::asSmallInt()
	{
		return Local<TSmallInt>(this->value.si);
	}

	Local<Array> Local<Object>::asArray()
	{
		return Local<Array>(this->isolate, reinterpret_cast<Array*>(this->value.gc));
	}

	Local<Map> Local<Object>::asMap()
	{
		return Local<Map>(this->isolate, reinterpret_cast<Map*>(this->value.gc));
	}

}
