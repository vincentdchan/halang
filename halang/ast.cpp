#include "stdafx.h"
#include "ast.h"
#include "codegen.h"

namespace halang
{
	void Node::visit(CodeGen *cg, CodePack* cp)
	{
		cg->visit(cp, this);
	}
};