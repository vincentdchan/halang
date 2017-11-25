#pragma once
#include <queue>
#include <memory>
#include <vector>
#include "token.h"
#include "util.h"
#include "StringBuffer.h"

namespace halang
{
	using namespace std;

	class Lexer: 
		public StringBuffer, 
		public utils::MessageContainer
	{
	public:
		typedef shared_ptr<string> pString;
		// Lexer();
		// void buffer(U16String _buf);
		Lexer();

		Token* NextToken();
		bool Match(Token::TYPE _type);
		Token* Eat(Token::TYPE _type);
		Token* PeekToken();
		// void GoBackToken();

		bool IsFinished() const {
			return is_finished;
		}

		// Disable
		Lexer(const Lexer&) = delete;
		Lexer& operator=(const Lexer&) = delete;

		void StartToken();

	protected:
		Token* current_tok;

	private:
		bool is_finished;

		Token* scanLiteral();
		Token* scanNumber();
		Token* scanString();
		void SwallowComment();

		Token* ReadToken();

		Token* MakeToken(Token::TYPE _type);
		Token* MakeToken(
			Token::TYPE _type,
			const U16String&
		);
		Token* MakeToken(double val);

		Location _start_location;

		std::vector<std::unique_ptr<Token>> token_buffer;
	};

}
