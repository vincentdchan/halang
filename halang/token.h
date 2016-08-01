#pragma once
#include <iostream>
#include <list>
#include "halang.h"


namespace halang
{

	using namespace std;

	struct Location
	{
		Location(int _line = -1, int _col = -1, int _len = -1): 
			line(_line), column(_col), length(_len) {}
		int line;
		int column, length;
	};

	struct Token
	{
#define EXTEND_OP(NAME, STR, PRECEDENCE) NAME##,
#define EXTEND_TOKEN(NAME) NAME##,
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

		union
		{
			double _double;
			std::string* _literal;
		};

		Token(): type(ILLEGAL), maybeInt(false)
		{}

		Token(const Token& _target)
		{
			type = _target.type;
			location = _target.location;
			maybeInt = _target.maybeInt;

			if (type == TYPE::NUMBER)
				_double = _target._double;
			else if (type == TYPE::IDENTIFIER)
				_literal = _target._literal;
		}

		Token(TYPE _t, Location _loc)
		{
			type = _t;
			location = _loc;
		}


		Token(Token&& _target)
		{
			type = _target.type;
			location = _target.location;
			maybeInt = _target.maybeInt;

			if (type == TYPE::NUMBER)
				_double = _target._double;
			else if (type == TYPE::IDENTIFIER)
				_literal = _target._literal;
		}

		static bool isOperator(const Token& t)
		{
			return t.type >= ADD && t.type < ILLEGAL_OP;
		}

		static OperatorType toOperator(const Token& t)
		{
			if (t.type >= Token::ADD && t.type <= Token::ILLEGAL_OP)
				return static_cast<OperatorType>(t.type);
			else
				return OperatorType::ILLEGAL_OP;
		}

		Token& operator=(const Token& _target)
		{
			type = _target.type;
			location = _target.location;
			maybeInt = _target.maybeInt;

			if (type == TYPE::NUMBER)
				_double = _target._double;
			else if (type == TYPE::IDENTIFIER)
				_literal = _target._literal;

			return *this;
		}

		Token& operator=(Token&& _target)
		{
			type = _target.type;
			location = _target.location;
			maybeInt = _target.maybeInt;

			if (type == TYPE::NUMBER)
				_double = _target._double;
			else if (type == TYPE::IDENTIFIER)
				_literal = _target._literal;

			return *this;
		}

		bool operator==(const Token& t) const
		{
			return this->type == t.type;
		}

		bool operator!=(const Token& t) const
		{
			return this->type != t.type;
		}

		bool operator==(TYPE _t) const
		{
			return this->type == _t;
		}

		bool operator!=(TYPE _t) const
		{
			return this->type != _t;
		}

		operator bool() const
		{
			return type != TYPE::ILLEGAL;
		}

	};
}
