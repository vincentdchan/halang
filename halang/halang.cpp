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

const char* VERSION_INFO =
"Halang interpreter developint version\n"
"v - 0.0.2\n";

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

#define CHECK_ERROR_16(MC) do { \
	if (MC->hasError()) \
	{			\
		for (auto i = MC->getMessages().begin();  \
				i != MC->getMessages().end(); ++i) \
		{ \
			utils:: _MessageContainer<std::u16string>::OutputMsg(std::cout, *i); \
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
	Function* main_fun = nullptr;

	nvm = new StackVM();

	string filename;
	if (argc > 1)
	{
		std::string arg1(argv[1]);
		if (arg1 == "-v")
		{
			std::cout << VERSION_INFO;
			return 0;
		}
		else
			filename = arg1;
	}
	else
	{
		std::cout << VERSION_INFO;
		return 0;
	}

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
	main_fun = cg->generate(parser);
	if (cg->hasError())
	{
		for (auto i = cg->getMessages().begin(); 
				i != cg->getMessages().end(); ++i)
		{
			std::cout << utils::UTF16_to_UTF8(i->msg) << std::endl;
		}
		goto CLEAR_AND_EXIT; 
	}

	CLEAR_PTR(lexer);
	CLEAR_PTR(parser);
	CLEAR_PTR(cg);

	nvm->InitializeFunction(main_fun);
	nvm->CallFunction(main_fun, Value());

	CLEAR_AND_EXIT:

	CLEAR_PTR(lexer);
	CLEAR_PTR(parser);
	CLEAR_PTR(cg);
	CLEAR_PTR(nvm);

	// string ch;
	// cin >> ch;
    return 0;
}
