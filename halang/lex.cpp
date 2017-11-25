#include <cstring>
#include <string>
#include "lex.h"

namespace halang
{
	using namespace std;

	Lexer::Lexer()
	{
		current_tok = nullptr;
		is_finished = false;
	}

	bool Lexer::Match(Token::TYPE _type) {
		return current_tok->type == _type;
	}

	Token* Lexer::NextToken() {
		auto current = current_tok;
		if (!is_finished) {
			current_tok = ReadToken();
			if (current_tok->type == Token::TYPE::ENDFILE) {
				is_finished = true;
			}
		}
		return current;
	}

	Token* Lexer::ReadToken() {
		StartToken();
		char16_t ch = GetChar();

		if (IsDigit(ch))
		{
			return scanNumber();
		}
		else if (IsAlphabet(ch) || 
			ch == u'_')
		{
			return scanLiteral();
		}
		else if (ch == u'"')
			return scanString();
		else switch (ch)
		{
		case u'\t':
		case u'\n':
		case u'\r':
		case u' ': 
			NextChar(); 
			return ReadToken();
			break;
		case u'!':
			NextChar();
			return MakeToken(Token::TYPE::NOT);
		case u'@':
			NextChar();
			return MakeToken(Token::TYPE::AT);
		case u'.':
			NextChar();
			return MakeToken(Token::TYPE::DOT);
		case u',':
			NextChar();
			return MakeToken(Token::TYPE::COMMA);
		case u';':
			NextChar();
			return MakeToken(Token::TYPE::SEMICOLON);
		case u'(':
			NextChar();
			return MakeToken(Token::TYPE::OPEN_PAREN);
		case u')':
			NextChar();
			return MakeToken(Token::TYPE::CLOSE_PAREN);
		case u'{':
			NextChar();
			return MakeToken(Token::TYPE::OPEN_BRACKET);
		case u'}':
			NextChar();
			return MakeToken(Token::TYPE::CLOSE_BRACKET);
		case u'[':
			NextChar();
			return MakeToken(Token::TYPE::OPEN_SQUARE_BRACKET);
		case u']':
			NextChar();
			return MakeToken(Token::TYPE::CLOSE_SQUARE_BRACKET);
		case u'+':
			NextChar();
			return MakeToken(Token::TYPE::ADD);
		case u'-':
			NextChar();
			if (GetChar() == u'>') {
				NextChar();
				return MakeToken(Token::TYPE::ARROW);
			}
			return MakeToken(Token::TYPE::SUB);
		case u'*':
			NextChar();
			if (GetChar() == u'*') { // **
				NextChar();
				return MakeToken(Token::TYPE::POW);
			}
			return MakeToken(Token::TYPE::MUL);
		case u'/':
			NextChar();
			if (GetChar() == u'/')
			{
				NextChar();
				while (!IsLineBreak(GetChar())) {
					NextChar();
				}
				NextToken();
				return ReadToken();
			}
			else if (GetChar() == u'*')
			{
				NextChar();
				SwallowComment();
				return ReadToken();
			}
			else
			{
				return MakeToken(Token::TYPE::DIV);
			}
			break;
		case u'%':
			NextChar();
			return MakeToken(Token::TYPE::MOD);
		case u'>':
			NextChar();
			if (GetChar() == u'=') {
				NextChar();
				return MakeToken(Token::TYPE::GTEQ);
			}
			return MakeToken(Token::TYPE::GT);
		case u'<':
			NextChar();
			if (GetChar() == u'=') {
				NextChar();
				return MakeToken(Token::TYPE::LTEQ);
			}
			return MakeToken(Token::TYPE::LT);
		case u'=':
			NextChar();
			if (GetChar() == u'=')
			{
				NextChar();
				return MakeToken(Token::TYPE::EQ);
			}
			return MakeToken(Token::TYPE::ASSIGN);
		case u'&':
			NextChar();
			return MakeToken(Token::TYPE::LG_AND);
		case u'|':
			NextChar();
			return MakeToken(Token::TYPE::LG_OR);
		default:
			return MakeToken(Token::TYPE::ENDFILE);
		}

	}

	Token* Lexer::scanString()
	{
		U16String buffer;

		buffer.push_back(NextChar());
		while (GetChar() != u'"')
		{
			buffer.push_back(NextChar());
			if (GetChar() == u'\\')
			{
				NextChar();
			}
		}

		return MakeToken(Token::TYPE::STRING, buffer);
	}

	Token* Lexer::scanLiteral()
	{
		U16String buffer;

		buffer.push_back(NextChar());

		while (
			IsAlphabet(GetChar()) || 
			IsDigit(GetChar()) || 
			GetChar() == '_') {

			buffer.push_back(NextChar());
		}

		if (buffer == u"get")
		{
			return MakeToken(Token::TYPE::GET);
		}
		else if (buffer == u"getter")
		{
			return MakeToken(Token::TYPE::GETTER);
		}
		else if (buffer == u"set")
		{
			return MakeToken(Token::TYPE::SET);
		}
		else if (buffer == u"setter")
		{
			return MakeToken(Token::TYPE::SETTER);
		}
		else if (buffer == u"accessor")
		{
			return MakeToken(Token::TYPE::ACCESSOR);
		}
		else if (buffer == u"break")
		{
			return MakeToken(Token::TYPE::BREAK);
		}
		else if (buffer == u"class")
		{
			return MakeToken(Token::TYPE::CLASS);
		}
		else if (buffer == u"continue")
		{
			return MakeToken(Token::TYPE::CONTINUE);
		}
		else if (buffer == u"let")
		{
			return MakeToken(Token::TYPE::LET);
		}
		else if (buffer == u"if")
		{
			return MakeToken(Token::TYPE::IF);
		}
		else if (buffer == u"else")
		{
			return MakeToken(Token::TYPE::ELSE);
		}
		else if (buffer == u"while")
		{
			return MakeToken(Token::TYPE::WHILE);
		}
		else if (buffer == u"fun")
		{
			return MakeToken(Token::TYPE::FUN);
		}
		else if (buffer == u"def")
		{
			return MakeToken(Token::TYPE::DEF);
		}
		else if (buffer == u"and")
		{
			return MakeToken(Token::TYPE::AND);
		}
		else if (buffer == u"or")
		{
			return MakeToken(Token::TYPE::OR);
		}
		else if (buffer == u"do")
		{
			return MakeToken(Token::TYPE::DO);
		}
		else if (buffer == u"end")
		{
			return MakeToken(Token::TYPE::END);
		}
		else if (buffer == u"then")
		{
			return MakeToken(Token::TYPE::THEN);
		}
		else if (buffer == u"return")
		{
			return MakeToken(Token::TYPE::RETURN);
		} else {
			return MakeToken(Token::TYPE::IDENTIFIER, buffer);
		}
	}

	Token* Lexer::scanNumber()
	{
		std::u16string buffer;
		bool maybeInt = true;

		buffer.push_back(NextChar());

		while (IsDigit(GetChar()) || GetChar() == u'.')
		{
			if (GetChar() == u'.')
			{
				maybeInt = false;
			}
			buffer.push_back(NextChar());
		}
		auto tok = MakeToken(std::stod(utils::utf16_to_utf8(buffer)));
		tok->maybeInt = maybeInt;
		return tok;
	}

	void Lexer::SwallowComment()
	{
		while (true)
		{
			if (GetChar() == u'*') {
				NextChar();
				if (GetChar() == u'/') {
					NextChar();
					break;
				}
			}
		}
	}

	Token* Lexer::MakeToken(Token::TYPE _type) {
		token_buffer.push_back(
			std::make_unique<Token>(_type, _start_location)
		);
		return (token_buffer.end() - 1)->get();
	}

	Token* Lexer::MakeToken(
		Token::TYPE _type,
		const U16String& _string_literal
	) {
		token_buffer.push_back(
			std::make_unique<Token>(_type, _start_location)
		);
		auto ptr = (token_buffer.end() - 1)->get();
		ptr->SetLiteralValue(_string_literal);
		return ptr;
	}

	Token* Lexer::MakeToken(double val) {
		token_buffer.push_back(
			std::make_unique<Token>(
				Token::TYPE::NUMBER, 
				_start_location
			)
		);
		auto ptr = (token_buffer.end() - 1)->get();
		ptr->SetDoubleValue(val);
		return ptr;
	}

	void Lexer::StartToken() {
		_start_location = GetLocation();
	}

}
