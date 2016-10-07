#include "stdafx.h"
#include <cstring>
#include "lex.h"
#include <string>
#include <locale>
#include <codecvt>

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

		std::string u8Buffer;
		std::getline(ist, u8Buffer);

		buffer = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}
			.from_bytes(u8Buffer.data());

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
			else if (buffer[iter] == u'"')
				result = scanString();
			else switch (buffer[iter])
			{
			case u'\t':
			case u'\n':
			case u' ': ++iter; break;
			case u'!':
				PUSH_TOKEN(NOT);
				++iter; break;
			case u'@':
				PUSH_TOKEN(AT);
				++iter; break;
			case u'.':
				PUSH_TOKEN(DOT);
				++iter; break;
			case u',':
				PUSH_TOKEN(COMMA);
				++iter; break;
			case u';':
				PUSH_TOKEN(SEMICOLON);
				++iter; break;
			case u'(':
				PUSH_TOKEN(OPEN_PAREN);
				++iter; break;
			case u')':
				PUSH_TOKEN(CLOSE_PAREN);
				++iter; break;
			case u'{':
				PUSH_TOKEN(OPEN_BRAKET);
				++iter; break;
			case u'}':
				PUSH_TOKEN(CLOSE_BRAKET);
				++iter; break;
			case u'[':
				PUSH_TOKEN(OPEN_SQUARE_BRAKET);
				++iter; break;
			case u']':
				PUSH_TOKEN(CLOSE_SQUARE_BRAKET);
				++iter; break;
			case u'+':
				PUSH_TOKEN(ADD);
				++iter; break;
			case u'-':
				PUSH_TOKEN(SUB);
				++iter; break;
			case u'*':
				if (swallow(u"**"))
				{
					PUSH_TOKEN(POW);
					break;
				}
				else
					PUSH_TOKEN(MUL);
				++iter; break;
			case u'/':
				PUSH_TOKEN(DIV);
				++iter; break;
			case u'%':
				PUSH_TOKEN(MOD);
				++iter; break;
			case u'>':
				if (swallow(u">="))
				{
					PUSH_TOKEN(GTEQ);
					break;
				}
				else
					PUSH_TOKEN(GT);
				++iter; break;
			case u'<':
				if (swallow(u"<="))
				{
					PUSH_TOKEN(LTEQ);
					break;
				}
				else
					PUSH_TOKEN(LT);
				++iter; break;
			case u'=':
				if (swallow(u"=="))
				{
					PUSH_TOKEN(EQ);
					break;
				}
				else
					PUSH_TOKEN(ASSIGN);
				++iter; break;
			default:
				ReportError(std::u16string(u"Lexer error: unexpected charactor: ") + buffer[iter]);
				++iter; break;
			}
		}
		++linenumber;
		return result;
	}

	bool Lexer::swallow(const char16_t* _str)
	{
		std::u16string str = _str;
		bool result = true;

		for (std::size_t i = 0; i < str.size(); ++i)
			if (i + iter >= buffer.size() || str[i] != buffer[iter + i])
			{
				result = false;
				break;
			}

		if (result)
			iter += str.size();
		return result;
	}

	bool Lexer::scanString()
	{
		auto ic = iter;
		if (buffer[ic++] != u'"') return false;
		std::u16string str;
		while (ic < buffer.size() && buffer[ic] != u'"')
		{
			if (buffer[ic] == u'\\')
			{
				if (ic + 1 < buffer.size() && buffer[ic + 1] == u'"')
				{
					str.push_back(u'"');
					ic += 2;
					continue;
				}
				else
					str.push_back(buffer[ic]);
			}
			else
				str.push_back(buffer[ic]);
			ic++;
		}
		if (buffer[ic++] == u'"')
		{
			Token t;
			t.location = loc;
			t.type = Token::TYPE::STRING;
			t._literal = make_literal();
			*t._literal = str;
			token_q.push(t);

			iter = ic;
			return true;
		}
		else
			return false;
	}

	bool Lexer::scanLiteral()
	{
		bool match = false;
		loc.column = iter;
		// check reserved
		switch (buffer[iter])
		{
		case 'c':
			if (match == swallow(u"class"))
			{
				loc.length = 5;
				PUSH_TOKEN(CLASS);
			}
			break;
		case 'v': //  var
			if (match = swallow(u"var"))
			{
				loc.length = 3;
				PUSH_TOKEN(VAR);
			}
			break;
		case 'i': // if
			if (match = swallow(u"if"))
			{
				loc.length = 2;
				PUSH_TOKEN(IF);
			}
			break;
		case 'e': // else
			if (match = swallow(u"else"))
			{
				loc.length = 4;
				PUSH_TOKEN(ELSE);
			}
			break;
		case 'w': // while
			if (match = swallow(u"while"))
			{
				loc.length = 5;
				PUSH_TOKEN(WHILE);
			}
			break;
		case 'f': // func
			if (match = swallow(u"func"))
			{
				loc.length = 4;
				PUSH_TOKEN(FUNC);
			}
			break;
		case 'r': // return
			if (match = swallow(u"return"))
			{
				loc.length = 6;
				PUSH_TOKEN(RETURN);
			}
			break;
		case 'p': //print
			if (match = swallow(u"print"))
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
			while (isDigit(buffer[ic]) || buffer[ic] == u'.')
			{
				if (buffer[ic] == u'.')
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
						t.location = loc;
						token_q.push(t);

						if (match)
							iter = ic; // watch out
						return match;
					}
					else
					{
						ReportError(u"Not a valid Number", Location(linenumber, ic, ic));
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
			t.location = loc;
			token_q.push(t);
		}
		else
		{
			match = false;
			ReportError(u"Not a valid number", Location(linenumber, ic, ic));
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

	bool Lexer::isDigit(char16_t ch)
	{
		return ch >= '0' && ch <= '9' ? true : false;
	}

	bool Lexer::isAlphabet(char16_t ch)
	{
		return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
	}

}
