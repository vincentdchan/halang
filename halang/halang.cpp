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
#include "string.h"
#include "svm_codes.h"
#include "svm.h"
#include "codegen.h"

const char* DEFAULT_FILENAME = "source.txt";

int main(int argc, char** argv)
{
	using namespace halang;

	auto nvm = new StackVM();

	char *filename;
	if (argc > 1)
		filename = argv[1];
	else
		filename = const_cast<char*>(DEFAULT_FILENAME);

	fstream fs;
	fs.open(filename, fstream::in);
	if (fs.fail())
	{
		std::cout << "input source not found." << std::endl;
		return 0;
	}
	Lexer* lexer = new Lexer(fs);

	if (lexer->hasError())
	{
		for (auto i = lexer->getMessages().begin(); i != lexer->getMessages().end(); ++i)
		{
			std::cout << *i;
		}
	}

	auto parser = new Parser(*lexer);
	parser->parse();

	fs.close();

	delete lexer; // release lexer resources after parsing
	lexer = nullptr;

	if (!parser->hasError())
	{
		CodeGen cg(nvm);
		cg.generate(parser);
		delete parser;
		cg.load();

		nvm->execute();
	}
	else
	{
		for (auto i = parser->getMessages().begin(); i != parser->getMessages().end(); ++i)
		{
			std::cout << *i;
		}
	}

	delete nvm;
	nvm = nullptr;
	string ch;
	cin >> ch;
    return 0;
}
