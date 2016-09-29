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
	Object:
		 - Immutable Object
			- small int
			- double
			- Refer Count Object
				- Imutable String		// for string constang, string splice and so on
				- Big Integer

		- GC Object
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
	class Class;
	class IString;
	class Map;
	class Array;

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
		// explicit Object(TBool _bl) : type(TYPE::BOOL) { value.bl = _bl; }
		// explicit Object(const char *_Str) { value.str = new IString(_Str); }
		explicit Object(IString _isp) : type(TYPE::STRING) { value.str = new IString(_isp); }
		Object& operator=(const Object&);
		Object& operator=(Object&&);

		inline bool isNul() const { return type == TYPE::NUL; }
		inline bool isGCObject() const { return type == TYPE::GC; }
		inline bool isMap() const { return type == TYPE::MAP; }
		inline bool isArray() const { return type == TYPE::ARRAY; }
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
}

namespace std
{

	template<>
	struct hash<halang::Object>
	{

		std::size_t operator()(const halang::Object& obj) const
		{
			switch (obj.type)
			{
			case halang::Object::TYPE::STRING:
			{
				auto str = reinterpret_cast<halang::IString*>(obj.value.str);
				return std::hash<halang::IString>()(*str);
			}
			case halang::Object::TYPE::SMALL_INT:
			{
				auto i = obj.value.si;
				return std::hash<halang::TSmallInt>()(i);
			}
			case halang::Object::TYPE::NUMBER:
			{
				auto num = obj.value.number;
				return std::hash<halang::TNumber>()(num);
			}
			default:
				throw std::runtime_error("You can only calculate hash for ");
			}
		}
	};

}

namespace halang
{

	/// <summary>
	/// A map is a base data structure in halang
	/// it's used inner to be the base of the GeneralObject
	/// </summary>
	class Map : public GCObject
	{
	public:

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
		inline Object At(std::size_t index) { return this->operator[](index); }

		void Push(Object obj);
		void SetValue(std::size_t index, Object);
		Object Pop();
		void RemoveAt(std::size_t);

		virtual Map* GetPrototype() override { return _proto; }
		virtual void SetPrototype(Map* pt) override { _proto = reinterpret_cast<GeneralObject*>(pt); }

	private:

		GeneralObject* _proto;

	};

}
