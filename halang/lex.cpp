#include "stdafx.h"
#include "lex.h"

namespace lex
{
	using namespace std;

	Lexer::Lexer(istream& s): 
		_end(false), linenumber(0), _beginpos(0), _endpos(0), ist(s)
	{
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

	bool Lexer::readline()
	{
		if (ist.eof()) return false;
		/*
		Token _token;
		*/

		string line;
		std::getline(ist, line);
		unsigned int i = 0;
		while (i < line.size())
		{
			Location loc;
			loc.line = linenumber;
			loc.begin = loc.end = i;
			switch (line[i])
			{
			case '\t':
			case '\n':
			case ' ': ++i; break;
			case '(':
				token_q.push(Token(Token::TYPE::OPEN_PAREN, loc));
				++i; break;
			case ')':
				token_q.push(Token(Token::TYPE::CLOSE_PAREN, loc));
				++i; break;
			case '{':
				token_q.push(Token(Token::TYPE::OPEN_BRAKET, loc));
				++i; break;
			case '}':
				token_q.push(Token(Token::TYPE::CLOSE_BRAKET, loc));
				++i; break;
			case '+':
				token_q.push(Token(Token::TYPE::ADD, loc));
				++i; break;
			case '-':
				token_q.push(Token(Token::TYPE::SUB, loc));
				++i; break;
			case '*':
				if (i + 1 != line.size() && line[i + 1] != '*')
					token_q.push(Token(Token::TYPE::MUL, loc));
				else
				{
					token_q.push(Token(Token::TYPE::POW, loc));
					++i;
				}
				++i; break;
			case '/':
				token_q.push(Token(Token::TYPE::DIV, loc));
				++i; break;
			case '%':
				token_q.push(Token(Token::TYPE::MOD, loc));
				++i; break;
			case '>':
				if (line[i + 1] == '=')
				{
					token_q.push(Token(Token::TYPE::GTEQ, loc));
					i += 2;
				}
				else
				{
					token_q.push(Token(Token::GT, loc));
					++i; 
				}
				break;
			case '<':
				if (line[i + 1] == '=')
				{
					token_q.push(Token(Token::TYPE::LTEQ, loc));
					i += 2; break;
				}
				else
				{
					token_q.push(Token(Token::LT, loc));
					++i; break;
				}
			case 'l': // let
				scanLet(line, i);
				break;
			case 'i': // if
				scanIf(line, i);
				break;
			case 'e': // else
				scanElse(line, i);
			case 'w': // while
				scanWhile(line, i);
				break;
			case 'd': // def
				scanDef(line, i);
				break;
			case'=':
				if (line[i + 1] == '=')
				{
					token_q.push(Token(Token::TYPE::EQ, loc));
					i += 2; break;
				}
				else
				{
					token_q.push(Token(Token::TYPE::ASSIGN, loc));
					++i; break;
				}
			default:
				if (isAlphabet(line[i]) || line[i] == '_')
					scanIdentifier(line, i);
				else if (isDigit(line[i]))
					scanNumber(line, i);
				else
				{
					ReportError(string("Lexer error: unexpected charactor: ") + line[i]);
					i++; break;
				}
			}
		}
		++linenumber;
		return true;
	}

	void Lexer::scanLet(string& str, unsigned int& ic)
	{
		if (str[ic] == 'l' && str[ic + 1] == 'e' && str[ic + 2] == 't')
		{
			token_q.push(Token(Token::TYPE::LET, Location(linenumber, ic, ic + 2)));
			ic += 3;
		}
		else
		{
			ReportError("Expected \"let\".", Location(linenumber, ic, ic+2));
		}
	}

	void Lexer::scanIf(string& str, unsigned int& ic)
	{
		if (str[ic] == 'i' && str[ic + 1] == 'f')
		{
			token_q.push(Token(Token::TYPE::IF, Location(linenumber, ic, ic + 1)));
			ic += 2;
		}
		else
		{
			ReportError("Expected \"if\".", Location(linenumber, ic, ic + 1));
		}
	}

	void Lexer::scanElse(string& str, unsigned int& ic)
	{
		if (str[ic] == 'e' &&
			str[ic + 1] == 'l' &&
			str[ic + 2] == 's' &&
			str[ic + 3] == 'e')
		{
			token_q.push(Token(Token::TYPE::ELSE, Location(linenumber, ic, ic + 3)));
			ic += 4;
		}
		else
		{
			ReportError("Expect \"else\".");
		}
	}

	void Lexer::scanWhile(string& str, unsigned int &ic)
	{
		if (str[ic] == 'w' &&
			str[ic + 1] == 'h' &&
			str[ic + 2] == 'i' &&
			str[ic + 3] == 'l' &&
			str[ic + 4] == 'e')
		{
			token_q.push(Token(Token::WHILE, Location(linenumber, ic, ic + 4)));
			ic += 5;
		}
		else
		{
			ReportError("Expect \"while\".");
		}
	}

	void Lexer::scanDef(string& str, unsigned int& ic)
	{
		if (str[ic] == 'd' && str[ic + 1] == 'e' && str[ic + 2] == 'f') 
		{
			token_q.push(Token(Token::DEF, Location(linenumber, ic, ic + 2)));
			ic += 3;
		}
		else
		{
			ReportError("Expect \"def\".");
		}
	}

	void Lexer::scanIdentifier(string& str, unsigned int& ic)
	{
		if (isAlphabet(str[ic]) || str[ic] == '_')
		{
			const unsigned int _begin = ic;
			Token _t(Token::TYPE::IDENTIFIER, Location(linenumber, _begin, ic-1));
			_t._literal = make_literal();
			_t._literal->push_back(str[ic]);
			ic++;
			while (isDigit(str[ic]) || isAlphabet(str[ic]) || str[ic] == '_')
			{
				_t._literal->push_back(str[ic]);
				ic++;
			}
			token_q.push(_t);
		}
		else
		{
			ReportError("Not a valid identifier.", Location(linenumber, ic, ic));
		}
	}

	void Lexer::scanNumber(string& str, unsigned int& ic)
	{
		Token t;
		Location &loc = t.location;
		loc.begin = loc.end = ic;
		loc.line = linenumber;

		string num;
		if (isDigit(str[ic]))
		{
			t.type = Token::TYPE::NUMBER;
			// t.pLiteral.reset(new string());
			num.push_back(str[ic++]);
			while (isDigit(str[ic]) || str[ic] == '.')
			{
				if (str[ic] == '.')
				{
					t.maybeInt = false;
					num.push_back(str[ic++]);
					if (isDigit(str[ic]))
					{
						while (isDigit(str[ic]))
							num.push_back(str[ic++]);
						loc.end = ic - 1;
						t._double = std::stod(num);
						token_q.push(t);
						return;
					}
					else
					{
						ReportError("Not a valid Number", Location(linenumber, ic, ic));
					}
				}
				else // digit
				{
					num.push_back(str[ic++]);
				}
			}
			loc.end = ic - 1;
			t.maybeInt = true;
			t._double = std::stod(num);
			token_q.push(t);
		}
		else
		{
			ReportError("Not a valid number", Location(linenumber, ic, ic));
		}
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
