#pragma once
#include <iostream>
#include <sstream>

namespace lex
{
	using namespace std;
	class Scanner
	{
	public:
		Scanner(std::istream&);
		bool readline(std::string& str)
		{
			return getline(ist, str).eof() != true;
		}
	private:
		istream& ist;
	};

	Scanner::Scanner(istream& in) : ist(in)
	{ }

}

