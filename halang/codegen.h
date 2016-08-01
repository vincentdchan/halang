#pragma once
#include <map>
#include <vector>
#include "ast.h"
#include "util.h"
#include "parser.h"
#include "svm.hpp"
#include "function.h"
#include "object.h"

namespace halang
{

	class CodeGen final : public utils::_MessageContainer
	{
	public:
		CodeGen(Parser&);
		CodeGen(const CodeGen&) = delete;
		CodeGen& operator=(const CodeGen&) = delete;
		void generate();

		void visit(CodePack*, Node*);
#define VISIT_METHOD(NAME) void visit(CodePack*, NAME##Node*);
		NODE_LIST(VISIT_METHOD)
#undef VISIT_METHOD

	private:

		Parser& _parser;
		CodePack *top_cp, *global_cp;


		void generate(CodePack*);

	};

};
