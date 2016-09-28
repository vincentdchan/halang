#pragma once
#include <map>
#include <cmath>
#include <ostream>
#include <string>
#include "halang.h"
#include "string.h"


namespace halang
{
	/*
	Object:

		 - Immutable Object
			- small int
			- double
			- Refer Count Object
				- Imutable String		// for string constang, string splice and so on
				- Big Integer

		- Mutable Object				// GC Object
			- CodePack
			- Function
			- HashMap
				- GenericObject
					- Array
					- Class

			- Mutable String			// for string to edit
				- String
				- pointer to Immutable String	// from String Pool

	*/

#define OBJ_LIST(V) \
	V(NUL) \
	V(GC) \
	V(UPVALUE) \
	V(DICT) \
	V(STRING) \
	V(CODE_PACK) \
	V(FUNCTION) \
	V(SMALL_INT) \
	V(NUMBER) \
	V(BOOL)

	class Object;

	class GCObject
	{
	public:
		GCObject* next;
		bool marked;
		virtual ~GCObject() {};
	};

	union _Value
	{
		GCObject* gc;
		IString *str;
		TSmallInt si;		// small int
		TNumber number;
		TBool bl;
	};

	/********************************/
	/*  do not use virtual method   */
	/*  i don't want to generate a  */
	/*  v-tables                    */
	/********************************/

	TNumber toNumber(Object _obj);

	class Object
	{
	public:
#define E(NAME) NAME##,
		enum class TYPE
		{
			OBJ_LIST(E)
		};
#undef E
		_Value value;
		TYPE type;

		Object() : type(TYPE::NUL) { value.gc = nullptr; }
		Object(const Object&);
		Object(Object&&);
		explicit Object(GCObject* _object, TYPE _t) : type(_t) { value.gc = _object; }
		explicit Object(TSmallInt _int) : type(TYPE::SMALL_INT) { value.si = _int; }
		explicit Object(TNumber _num) : type(TYPE::NUMBER) { value.number = _num; }
		explicit Object(TBool _bl) : type(TYPE::BOOL) { value.bl = _bl; }
		// explicit Object(const char *_Str) { value.str = new IString(_Str); }
		explicit Object(IString _isp) : type(TYPE::STRING) { value.str = new IString(_isp); }
		Object& operator=(const Object&);
		Object& operator=(Object&&);

		inline bool isNul() const { return type == TYPE::NUL; }
		inline bool isGCObject() const { return type == TYPE::GC; }
		inline bool isDict() const { return type == TYPE::DICT; }
		inline bool isString() const { return type == TYPE::STRING; }
		inline bool isSmallInt() const { return type == TYPE::SMALL_INT; }
		inline bool isNumber() const { return type == TYPE::NUMBER; }
		inline bool isBool() const { return type == TYPE::BOOL; }

		inline void check_delete_str()
		{
			if (type == TYPE::STRING)
			{
				delete value.str;
				type = TYPE::NUL;
			}
		}

		inline void setNull() 
		{ 
			check_delete_str();
			value.gc = nullptr;
			type = TYPE::NUL; 
		}

		inline void setGCObject(GCObject* _obj)
		{
			check_delete_str();
			value.gc = _obj;
			type = TYPE::GC;
		}

		inline void setString(const IString& _str)
		{
			check_delete_str();
			value.str = new IString(_str);
			type = TYPE::STRING;
		}

		inline void setNumber(TNumber num)
		{
			check_delete_str();
			value.number = num;
			type = TYPE::NUMBER;
		}

		inline void setSmallInt(TSmallInt num)
		{
			check_delete_str();
			value.si = num;
			type = TYPE::SMALL_INT;
		}

		inline void setBool(TBool _bl)
		{
			check_delete_str();
			value.bl = _bl;
			type = TYPE::BOOL;
		}

#define GETVALUE(OBJ) (OBJ.isSmallInt() ? OBJ.value.si : toNumber(OBJ))
		Object applyOperator(OperatorType op, Object that = Object()) const
		{
			switch (op)
			{
			case OperatorType::NOT:
				return Object(!static_cast<bool>(*this));
			case OperatorType::ADD:
				return Object(GETVALUE((*this)) + GETVALUE(that));
			case OperatorType::SUB:
				return Object(GETVALUE((*this)) - GETVALUE(that));
			case OperatorType::MUL:
				return Object(GETVALUE((*this)) * GETVALUE(that));
			case OperatorType::DIV:
				return Object(GETVALUE((*this)) / GETVALUE(that));
			case OperatorType::MOD:
				if (isSmallInt() && that.isSmallInt())
					return Object(value.si % that.value.si);
				else if ((isSmallInt() || isNumber()) && (that.isSmallInt() || that.isNumber()))
					return Object(fmod(toNumber(*this), toNumber(that)));
				break;
			case OperatorType::POW:
				if ((isSmallInt() || isNumber()) && (that.isSmallInt() || that.isNumber()))
					return Object(pow(toNumber(*this), toNumber(that)));
				break;
			case OperatorType::GT:
				return Object(GETVALUE((*this)) > GETVALUE(that));
			case OperatorType::LT:
				return Object(GETVALUE((*this)) < GETVALUE(that));
			case OperatorType::GTEQ:
				return Object(GETVALUE((*this)) >= GETVALUE(that));
			case OperatorType::LTEQ:
				return Object(GETVALUE((*this)) <= GETVALUE(that));
			case OperatorType::EQ:
				return Object(GETVALUE((*this)) == GETVALUE(that));
			}
			return Object();
		}

		operator bool() const
		{
			if (isBool())
				return value.bl;
			else if (isSmallInt())
				return value.si != 0;
			else if (isNumber())
				return abs(value.number) > 0.0000001;
			else if (isNul())
				return false;
			else
				return true;
		}

		~Object()
		{
			if (type == TYPE::STRING)
				delete value.str;
		}

	};

	static const char* STRUE = "True";
	static const char* SFALSE = "False";
	std::ostream& operator<<(std::ostream& _ost, Object _obj);

	inline Object make_null()
	{
		return Object();
	}

	class StateObject : public GCObject
	{
	public:
		StateObject() : gc_root(nullptr)
		{}
		std::map<std::string, Object> variables;
		GCObject* gc_root;

		~StateObject()
		{
			iteratorRelease(gc_root);
		}
	private:
		void iteratorRelease(GCObject* tree)
		{
			if (tree == nullptr) return;
			iteratorRelease(tree->next);
			delete tree;
		}
	};

}
