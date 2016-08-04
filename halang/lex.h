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

	class Lexer final: public utils::_MessageContainer
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
		static bool isDigit(uc32 t);
		static bool isAlphabet(uc32 t);
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
		bool swallow(const char* _str);

		queue<Token> token_q;
		// queue<string> literal_q;
		bool _end;
		unsigned int linenumber, _beginpos, _endpos;
		// stream
		istream &ist;

		string buffer;
		std::size_t iter;
		Location loc;

		std::list<std::string*> _literal_list;

		template<typename... _Types>
		inline std::string* make_literal(_Types... Args)
		{
			std::string* _str = new std::string(std::forward<_Types>(Args)...);
			_literal_list.push_back(_str);
			return _str;
		}

	};

}
