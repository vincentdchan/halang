#pragma once
#include "ast.h"
#define E(Name) class Name##Node;
	NODE_LIST(E)
#undef E

#define VISIT_OVERRIDE virtual void visit(CodeGen *cg, CodePack* cp) override;

namespace halang
{

	class Visitor
	{

#define E(Name) virtual void visit(Name##Node*) = 0;
		NODE_LIST(E)
#undef E

	};

}
