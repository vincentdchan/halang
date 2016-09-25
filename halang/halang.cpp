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
#include "util.h"

const char* DEFAULT_FILENAME = "source.txt";

#define CHECK_ERROR(MC) do { \
	if (MC->hasError()) \
	{			\
		for (auto i = MC->getMessages().begin();  \
				i != MC->getMessages().end(); ++i) \
		{ \
			utils:: _MessageContainer<std::string>::OutputMsg(std::cout, *i); \
		} \
		goto CLEAR_AND_EXIT; \
	} \
} while(0)

#define CLEAR_PTR(PTR) if ((PTR) != nullptr) \
	{ \
		delete (PTR); \
		(PTR) = nullptr; \
	}


int main(int argc, char** argv)
{
	using namespace halang;
	Lexer *lexer = nullptr;
	Parser *parser = nullptr;
	CodeGen *cg = nullptr;
	StackVM *nvm = nullptr;

	nvm = new StackVM();

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
	lexer = new Lexer(fs);

	CHECK_ERROR(lexer);

	parser = new Parser(*lexer);
	parser->parse();
	fs.close();

	CHECK_ERROR(parser);

	cg = new CodeGen(nvm);
	cg->generate(parser);
	CHECK_ERROR(cg);

	cg->load();
	CLEAR_PTR(lexer);
	CLEAR_PTR(parser);
	CLEAR_PTR(cg);

	nvm->execute();

	CLEAR_AND_EXIT:

	CLEAR_PTR(lexer);
	CLEAR_PTR(parser);
	CLEAR_PTR(cg);
	CLEAR_PTR(nvm);

	string ch;
	cin >> ch;
    return 0;
}
