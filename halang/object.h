#pragma once
#include <map>
#include <cmath>
#include <ostream>
#include <string>
#include "halang.h"


namespace halang
{

#define OBJ_LIST(V) \
	V(NUL) \
	V(GC) \
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
	};

	union _Value
	{
		GCObject* gc;
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

		explicit Object() : type(TYPE::NUL) { value.gc = nullptr; }
		explicit Object(GCObject* _object, TYPE _t) : type(_t) { value.gc = _object; }
		explicit Object(TSmallInt _int) : type(TYPE::SMALL_INT) { value.si = _int; }
		explicit Object(TNumber _num) : type(TYPE::NUMBER) { value.number = _num; }
		explicit Object(TBool _bl) : type(TYPE::BOOL) { value.bl = _bl; }

		inline bool isNul() const { return type == TYPE::NUL; }
		inline bool isGCObject() const { return type == TYPE::GC; }
		inline bool isString() const { return type == TYPE::STRING; }
		inline bool isSmallInt() const { return type == TYPE::SMALL_INT; }
		inline bool isNumber() const { return type == TYPE::NUMBER; }
		inline bool isBool() const { return type == TYPE::BOOL; }

		inline void setNull() 
		{ 
			value.gc = nullptr;
			type = TYPE::NUL; 
		}
		inline void setGCObject(GCObject* _obj)
		{
			value.gc = _obj;
			type = TYPE::GC;
		}
		inline void setString(GCObject* _str)
		{
			value.gc = _str;
			type = TYPE::STRING;
		}
		inline void setNumber(TNumber num)
		{
			value.number = num;
			type = TYPE::NUMBER;
		}
		inline void setSmallInt(TSmallInt num)
		{
			value.si = num;
			type = TYPE::SMALL_INT;
		}

		inline void setBool(TBool _bl)
		{
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

		/*
		Object compareTo(Object that) const
		{
			return Object(0);
		}

		Object equalTo(Object that) const
		{
			return Object(0);
		}
		*/
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
