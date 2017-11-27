#pragma once
#include <iostream>
#include <list>
#include <memory>
#include "halang.h"


namespace halang
{

	using namespace std;

	struct Location
	{
		Location(int _line = -1, int _col = -1, int _pos = -1): 
			line(_line), column(_col), pos(_pos) {}
		int line;
		int column;
		int pos;
	};

	class Token final
	{
	public:
#define EXTEND_OP(NAME, STR, PRECEDENCE) NAME,
#define EXTEND_TOKEN(NAME) NAME,
		enum TYPE
		{
			OPERATOR_LIST(EXTEND_OP)
			TOKEN_LIST(EXTEND_TOKEN)
			TOKEN_NUMBER
		};
#undef EXTEND_OP
#undef EXTEND_TOKEN

		TYPE type;
		bool maybeInt;
		Location location;

	private:
		// Number* | String*
		double double_value;
		U16String literal_value;

	public:

		Token();
		Token(TYPE _t, Location _loc);

		inline void
		SetLiteralValue(const U16String& literal) {
			literal_value = literal;
		}

		inline 
		const U16String&
		GetLiteralValue() {
			return literal_value;
		}

		inline void
		SetDoubleValue(double val) {
			double_value = val;
		}

		inline
		double GetDoubleValue() {
			return double_value;
		}

		static bool IsOperator(const Token& t)
		{
			return t.type >= ADD && t.type < ILLEGAL_OP;
		}

		static OperatorType ToOperator(const Token& t)
		{
			if (t.type >= Token::ADD && t.type <= Token::ILLEGAL_OP)
				return static_cast<OperatorType>(t.type);
			else
				return OperatorType::ILLEGAL_OP;
		}

		inline bool operator==(const Token& t) const
		{
			return this->type == t.type;
		}

		inline bool operator!=(const Token& t) const
		{
			return this->type != t.type;
		}

		inline bool operator==(TYPE _t) const
		{
			return this->type == _t;
		}

		inline bool operator!=(TYPE _t) const
		{
			return this->type != _t;
		}

		inline operator bool() const
		{
			return type != TYPE::ILLEGAL;
		}

		const std::string& 
		ToString() const;

	};
}
