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
	class IString;
	class Map;
	class Array;

	class Isolate;


	union _Value
	{
		GCObject* gc;
		IString *str;
		TSmallInt si;		// small int
		TNumber number;
		TBool bl;
	};

	class Object
	{
	public:
		TypeId typeId;
		TypeFlag typeFlag;

		Object() : typeId(TypeId::Null), typeFlag(TypeFlag::Normal) { }

		virtual Object* GetPrototype() const = 0;

		inline bool isNul() const { return typeId == TypeId::Null; }
		inline bool isGCObject() const { return typeId >= TypeId::GCObject; }
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

	protected:
		SmallInt(int32_t v) : value(v)
		{}

	private:
		int32_t value;
	};

	class Number : public Object
	{
	public:
		friend class GC;

		virtual Object* GetPrototype() const override { return nullptr; }

	protected:
		Number(double num) : value(num) {}

	private:
		double value;
	};

	/// <summary>
	/// A map is a base data structure in halang
	/// it's used inner to be the base of the GeneralObject
	/// </summary>
	class Map : public Object
	{

	};

}
