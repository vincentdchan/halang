#pragma once
#include <queue>
#include <istream>
#include <sstream>
#include <memory>
#include <list>
#include "token.h"
#include "util.h"

namespace lex
{
	using namespace std;

	class Lexer final: utils::_MessageContainer
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
		void scanNumber(string&, unsigned int&);
		void scanLet(string&, unsigned int&);
		void scanIf(string&, unsigned int&);
		void scanElse(string&, unsigned int&);
		void scanWhile(string&, unsigned int&);
		void scanDef(string&, unsigned int&);
		void scanIdentifier(string&, unsigned int&);
		queue<Token> token_q;
		// queue<string> literal_q;
		bool _end;
		unsigned int linenumber, _beginpos, _endpos;
		// stream
		istream &ist;

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
