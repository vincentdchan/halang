#pragma once
#include <queue>
#include <istream>
#include <sstream>
#include <memory>
#include <list>
#include "token.h"
#include "util.h"

namespace halang
{
	using namespace std;

	class Lexer final: public utils::_MessageContainer<std::u16string>
	{
	public:
		typedef shared_ptr<string> pString;
		Lexer(istream& s);
		Lexer(const Lexer&) = delete;
		Lexer& operator=(const Lexer&) = delete;
		Token read();
		inline void finish();
		bool isFinish()
		{
			return _end;
		}
		static bool isDigit(TChar t);
		static bool isAlphabet(TChar t);
		~Lexer()
		{
			for (auto i = _literal_list.begin(); i != _literal_list.end(); ++i)
			{
				delete *i;
			}
		}
	private:
		Lexer();
		bool readline();

		bool scanString();
		bool scanLiteral();
		bool scanIdentifier();
		bool scanNumber();
		bool swallow(const char16_t* _str);

		queue<Token> token_q;
		// queue<string> literal_q;
		bool _end;
		unsigned int linenumber, _beginpos, _endpos;
		// stream
		istream &ist;

		std::u16string buffer;
		std::size_t iter;
		Location loc;

		std::list<std::u16string*> _literal_list;

		template<typename... _Types>
		inline std::u16string* make_literal(_Types... Args)
		{
			std::u16string* _str = new std::u16string(std::forward<_Types>(Args)...);
			_literal_list.push_back(_str);
			return _str;
		}

	};

}
