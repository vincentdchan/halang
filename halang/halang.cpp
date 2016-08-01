// halang.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <utility>
#include <memory>
#include "lex.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"
#include "svm_codes.h"
#include "svm.hpp"
#include "codegen.h"

const char* DEFAULT_FILENAME = "source.txt";

int main(int argc, char** argv)
{
	using namespace halang;

	char *filename;
	if (argc > 1)
		filename = argv[1];
	else
		filename = const_cast<char*>(DEFAULT_FILENAME);

	fstream fs;
	fs.open(filename, fstream::in);
	Lexer* lexer = new Lexer(fs);

	if (lexer->hasError())
	{
		for (auto i = lexer->getMessages().begin(); i != lexer->getMessages().end(); ++i)
		{
			std::cout << *i;
		}
	}

	Parser parser(*lexer);
	parser.parse();

	fs.close();
	delete lexer; // release lexer after parsing
	lexer = nullptr;

	if (!parser.hasError())
	{
		CodeGen cg(parser);
		cg.generate();

		/*
		auto state = make_unique<State>(cg.pack.variablesSize, cg.pack.constant);
		auto nvm = make_unique<StackVM<vector<Instruction>::iterator> >(cg.pack.instructions.begin());
		nvm->setState(state.get());
		nvm->execute();
		*/
	}
	else
	{
		for (auto i = parser.getMessages().begin(); i != parser.getMessages().end(); ++i)
		{
			std::cout << *i;
		}
	}

	string ch;
	cin >> ch;
    return 0;
}

