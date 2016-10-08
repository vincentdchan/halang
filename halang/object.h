#pragma once
#include <string>
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
			- UpValue
			- String
			- Array
			- CodePack
			- Function
			- Dict // an hash map
				- General Object
					- Class				// to generate general object
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

		virtual Dict* GetPrototype() = 0;

	protected:

		GCObject* next;
		bool marked;

		virtual Value toValue() { return Value(); };
		virtual void Mark() {}
		virtual ~GCObject() {}
	};

	enum class TypeId {
		Null,
		Bool,
		SmallInt,
		Number,
		GCObject,
		CodePack,
		Function,
		UpValue,
		String,
		Array,
		Dict,
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

}
