#include "stdafx.h"
#include <cstring>
#include "lex.h"

namespace halang
{
	using namespace std;

	Lexer::Lexer(istream& s): 
		_end(false), linenumber(0), _beginpos(0), _endpos(0), ist(s)
	{
		loc.line = 0;
		readline();
	}

	Token Lexer::read()
	{
		if (!token_q.empty())
		{
			Token t = token_q.front();
			token_q.pop();
			return t;
		}
		else
		{
			if (readline())
				return read();
			else
				return Token(Token::TYPE::ENDFILE, Location(linenumber));
		}
	}

#define PUSH_TOKEN(TK_NAME) token_q.push(Token(Token::TYPE::TK_NAME, loc))

	bool Lexer::readline()
	{
		if (ist.eof()) return false;
		bool result = true;

		std::getline(ist, buffer);
		iter = 0;
		while (iter < buffer.size() && result)
		{
			loc.line = linenumber;
			loc.column = iter;

			if (isDigit(buffer[iter]))
			{
				result = scanNumber();
			}
			else if (isAlphabet(buffer[iter]))
			{
				result = scanLiteral();
			}
			else switch (buffer[iter])
			{
			case '\t':
			case '\n':
			case ' ': ++iter; break;
			case '!':
				PUSH_TOKEN(NOT);
				++iter; break;
			case '.':
				PUSH_TOKEN(DOT);
				++iter; break;
			case ',':
				PUSH_TOKEN(COMMA);
				++iter; break;
			case ';':
				PUSH_TOKEN(SEMICOLON);
				++iter; break;
			case '(':
				PUSH_TOKEN(OPEN_PAREN);
				++iter; break;
			case ')':
				PUSH_TOKEN(CLOSE_PAREN);
				++iter; break;
			case '{':
				PUSH_TOKEN(OPEN_BRAKET);
				++iter; break;
			case '}':
				PUSH_TOKEN(CLOSE_BRAKET);
				++iter; break;
			case '+':
				PUSH_TOKEN(ADD);
				++iter; break;
			case '-':
				PUSH_TOKEN(SUB);
				++iter; break;
			case '*':
				if (swallow("**"))
				{
					PUSH_TOKEN(POW);
					break;
				}
				else
					PUSH_TOKEN(MUL);
				++iter; break;
			case '/':
				PUSH_TOKEN(DIV);
				++iter; break;
			case '%':
				PUSH_TOKEN(MOD);
				++iter; break;
			case '>':
				if (swallow(">="))
				{
					PUSH_TOKEN(GTEQ);
					break;
				}
				else
					PUSH_TOKEN(GT);
				++iter; break;
			case '<':
				if (swallow("<="))
				{
					PUSH_TOKEN(LTEQ);
					break;
				}
				else
					PUSH_TOKEN(LT);
				++iter; break;
			case'=':
				if (swallow("=="))
				{
					PUSH_TOKEN(EQ);
					break;
				}
				else
					PUSH_TOKEN(ASSIGN);
				++iter; break;
			default:
				ReportError(string("Lexer error: unexpected charactor: ") + buffer[iter]);
				++iter; break;
			}
		}
		++linenumber;
		return result;
	}

	bool Lexer::swallow(const char* _str)
	{
		auto len = std::strlen(_str);
		bool result = true;
		for (std::size_t i = 0; i < len; ++i)
		{
			if (i + iter >= buffer.size() || _str[i] != buffer[iter + i])
			{
				result = false;
				break;
			}
		}
		if (result)
			iter += len;
		return result;
	}

	bool Lexer::scanLiteral()
	{
		bool match = false;
		loc.column = iter;
		// check reserved
		switch (buffer[iter])
		{
			case 'v': //  var
				if (match = swallow("var"))
				{
					loc.length = 3;
					PUSH_TOKEN(VAR);
				}
				break;
			case 'i': // if
				if (match = swallow("if"))
				{
					loc.length = 2;
					PUSH_TOKEN(IF);
				}
				break;
			case 'e': // else
				if (match = swallow("else"))
				{
					loc.length = 4;
					PUSH_TOKEN(ELSE);
				}
				break;
			case 'w': // while
				if (match = swallow("while"))
				{
					loc.length = 5;
					PUSH_TOKEN(WHILE);
				}
				break;
			case 'f': // func
				if (match = swallow("func"))
				{
					loc.length = 4;
					PUSH_TOKEN(FUNC);
				}
				break;
			case 'r': // return
				if (match = swallow("return"))
				{
					loc.length = 6;
					PUSH_TOKEN(RETURN);
				}
				break;
			case 'p': //print
				if (match = swallow("print"))
				{
					loc.length = 5;
					PUSH_TOKEN(PRINT);
				}
				break;
		}
		if (!match)
			match = scanIdentifier();
		return match;
	}

	bool Lexer::scanIdentifier()
	{
		auto ic = iter;

		if (!isAlphabet(buffer[ic]))
		{
			return false;
		}

		Token t;
		loc.column = iter;
		loc.length = 0;
		t.type = Token::TYPE::IDENTIFIER;
		t._literal = make_literal();

		while (isAlphabet(buffer[ic]) || buffer[ic] == '_')
		{
			loc.length++;
			t._literal->push_back(buffer[ic++]);
		}

		// finish work
		iter = ic;
		t.location = loc;
		token_q.push(t);
		return true;
	}

	bool Lexer::scanNumber()
	{
		Token t;

		// prepare works
		bool match = true;
		auto ic = iter;
		loc.column = iter;

		string num;
		if (isDigit(buffer[ic]))
		{
			t.type = Token::TYPE::NUMBER;
			// t.pLiteral.reset(new string());
			num.push_back(buffer[ic++]);
			while (isDigit(buffer[ic]) || buffer[ic] == '.')
			{
				if (buffer[ic] == '.')
				{
					t.maybeInt = false;
					num.push_back(buffer[ic++]);
					if (isDigit(buffer[ic]))
					{
						while (isDigit(buffer[ic])) {
							num.push_back(buffer[ic++]);
							++loc.length;
						}
						t._double = std::stod(num);
						token_q.push(t);

						if (match)
							iter = ic; // watch out
						return match;
					}
					else
					{
						ReportError("Not a valid Number", Location(linenumber, ic, ic));
						match = false;
					}
				}
				else // digit
				{
					num.push_back(buffer[ic++]);
					loc.length++;
				}
			}
			t.maybeInt = true;
			t._double = std::stod(num);
			token_q.push(t);
		}
		else
		{
			match = false;
			ReportError("Not a valid number", Location(linenumber, ic, ic));
		}
		if (match) // success
			iter = ic;
		return match;
	}

	void Lexer::finish()
	{
		token_q.push(Token(Token::TYPE::ENDFILE, Location(linenumber)));
		_end = true;
	}

	bool Lexer::isDigit(uc32 ch)
	{
		return ch >= '0' && ch <= '9' ? true : false;
	}

	bool Lexer::isAlphabet(uc32 ch)
	{
		return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
	}

}
