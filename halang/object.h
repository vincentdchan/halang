#pragma once
#include <unordered_map>
#include <cmath>
#include <ostream>
#include <string>
#include <exception>
#include <vector>
#include "halang.h"
#include "string.h"
#include "type.h"

// ******************************************
// Everything is Object
// ******************************************

#define DEFINE_CAST(NAME) static NAME* Cast(Object* obj) { \
	return reinterpret_cast<NAME*>(obj); \
}

namespace halang
{

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
	class GCObject;
	class Class;
	class String;
	class Array;
	class Dict;

	class Isolate;

	class Object
	{
	protected:
		TypeId typeId;
		TypeFlag typeFlag;

	public:
		friend class CodeGen;

		Object(TypeId _ti = TypeId::Null) : 
			typeId(_ti), typeFlag(TypeFlag::Normal) { }
		Object(const Object& obj) :
			typeId(obj.typeId), typeFlag(obj.typeFlag)
		{}

		inline TypeId GetTypeId() const { return typeId; }
		inline TypeFlag GetTypeFlag() const { return typeFlag; }
		virtual Object* GetPrototype() const = 0;
		virtual void Mark() {}

		inline bool isNul() const { return typeId == TypeId::Null; }
		inline bool isScriptContext() const { return typeId == TypeId::ScriptContext; }
		inline bool isMap() const { return typeId == TypeId::Map; }
		inline bool isDict() const { return typeId == TypeId::Dict; }
		inline bool isArray() const { return typeId == TypeId::Array; }
		inline bool isString() const { return typeId == TypeId::String; }
		inline bool isSmallInt() const { return typeId == TypeId::SmallInt; }
		inline bool isNumber() const { return typeId == TypeId::Number; }
		inline bool isBool() const { return typeId == TypeId::Bool; }

		inline void setNull()
		{
			typeId = TypeId::Null;
		}

	};

	class SmallInt : public Object
	{
	public:
		friend class GC;

		virtual Object* GetPrototype() const override { return nullptr; }

		DEFINE_CAST(SmallInt)

	protected:

		SmallInt(int32_t v) : value(v)
		{
			typeId = TypeId::SmallInt;
		}

	private:

		TSmallInt value;

	public:

		inline TSmallInt GetValue() const { return value; }

	};

	class Number : public Object
	{
	public:
		friend class GC;

		virtual Object* GetPrototype() const override { return nullptr; }

		DEFINE_CAST(Number)

	protected:

		Number(double num) : value(num) 
		{
			typeId = TypeId::Number;
		}

	private:
		TNumber value;

	public:

		inline TNumber GetValue() const { return value; }

	};

}
