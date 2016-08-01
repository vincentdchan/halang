#pragma once
#include <map>
#include <vector>
#include "ast.h"
#include "util.h"
#include "parser.h"
#include "svm.h"
#include "function.h"
#include "object.h"

namespace halang
{

	class CodeGen final : public utils::_MessageContainer
	{
	public:
		CodeGen(StackVM *_vm);
		CodeGen(const CodeGen&) = delete;
		CodeGen& operator=(const CodeGen&) = delete;
		void generate(Parser*);

		void load();
		void visit(CodePack*, Node*);
#define VISIT_METHOD(NAME) void visit(CodePack*, NAME##Node*);
		NODE_LIST(VISIT_METHOD)
#undef VISIT_METHOD

	private:

		StackVM* vm;
		Parser* parser;
		CodePack *top_cp, *global_cp;


		void generate(CodePack*);

	};

};
