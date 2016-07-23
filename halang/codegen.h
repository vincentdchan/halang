#pragma once
#include <map>
#include <vector>
#include "ast.h"
#include "parser.h"
#include "svm.hpp"
#include "object.h"

namespace codegen
{
	using namespace parser;
	using namespace runtime::StackVM;
	using namespace runtime;

	struct CodePack
	{
		std::size_t variablesSize;
		std::vector<Object> constant;
		std::vector<Instruction> instructions;
	};

	class CodeGen final
	{
	public:
		CodeGen(Parser&);
		CodeGen(const CodeGen&) = delete;
		CodeGen& operator=(const CodeGen&) = delete;
		void generate();
		CodePack pack;
	private:
		Parser& _parser;
		std::map<std::string, unsigned int> var_id;

		void visit(Node*);
#define VISIT_METHOD(NAME) void visit(NAME##Node*);
		NODE_LIST(VISIT_METHOD)
#undef VISIT_METHOD

	};

}
