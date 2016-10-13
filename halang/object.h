#pragma once
#include <string>
#include <vector>
#include <utility>
#include "halang.h"

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

	class Object;
	class String;
	class ConsString;
	class SliceString;
	class Array;
	class Dict;
	class Function;

	struct Value;

	class GCObject
	{
	public:

		friend class GC;

		virtual Dict* GetPrototype() { return nullptr; }
		virtual Value toValue();
		virtual void Mark() {}
		virtual ~GCObject() {}

	protected:

		GCObject* next;
		bool marked;

	};

	enum class TypeId {
		Null,
		Bool,
		SmallInt,
		Number,
		GCObject,
		ScriptContext,
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

		union
		{
			GCObject* gc;
			String *str;
			TSmallInt si;		// small int
			TNumber number;
			TBool bl;
		} value;
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
		inline bool isString() const { return type == TypeId::String; }
		inline bool isScriptContext() const { return type == TypeId::ScriptContext; }
		inline bool isCodePack() const { return type == TypeId::CodePack; }
		inline bool isFunction() const { return type == TypeId::Function; }
		inline bool isUpValue() const { return type == TypeId::UpValue; }
		inline bool isArray() const { return type == TypeId::Array; }
		inline bool isDict() const { return type == TypeId::Dict; }

		inline operator bool() const
		{
			switch (type)
			{
			case halang::TypeId::Null:
				return false;
			case halang::TypeId::Bool:
				return value.bl;
			case halang::TypeId::SmallInt:
				return value.si == 0;
			case halang::TypeId::Number:
				return value.number == 0;
			case halang::TypeId::GCObject:
			case halang::TypeId::ScriptContext:
			case halang::TypeId::CodePack:
			case halang::TypeId::Function:
			case halang::TypeId::UpValue:
			case halang::TypeId::String:
			case halang::TypeId::Array:
			case halang::TypeId::Dict:
			default:
				return false;
			}
		}

		bool operator==(const Value& that) const;

	};

}
