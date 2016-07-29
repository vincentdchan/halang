#pragma once
#include <map>
#include <vector>
#include "util.h"
#include "ast.h"
#include "parser.h"
#include "svm.hpp"
#include "object.h"

namespace codegen
{
	using namespace parser;
	using namespace runtime::StackVM;
	using namespace runtime;

	/*
	struct CodePack
	{
		std::size_t variablesSize;
		std::vector<Object> constant;
		std::vector<Instruction> instructions;
		std::map<std::size_t, std::string> var_names;
	};
	*/

	struct Environment
	{
		Environment() : parent(nullptr) {}
		Environment* parent;
		std::size_t var_size;
		std::map<std::string, std::size_t> var_id;
		std::map<std::size_t, std::string> var_names;
		std::vector<Object> constant;

		std::vector<Instruction> instructions;
		
		// if created is true, it will create new one if not found
		// return -1 if not found.
		int findLocalVariable(std::string _name, bool created); 	
	};

	class CodeGen final : public utils::_MessageContainer
	{
	public:
		CodeGen(Parser&);
		CodeGen(const CodeGen&) = delete;
		CodeGen& operator=(const CodeGen&) = delete;
		void generate();
		// CodePack pack;
	private:

		Parser& _parser;
		/*
		std::map<std::string, unsigned int> var_id;
		std::map<std::size_t, std::string> var_names;
		*/

		Environment* env;

		void visit(Node*);
#define VISIT_METHOD(NAME) void visit(NAME##Node*);
		NODE_LIST(VISIT_METHOD)
#undef VISIT_METHOD

	};

}
