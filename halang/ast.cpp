#include "stdafx.h"
#include "ast.h"
#include "codegen.h"

namespace halang
{
#define VIRTUAL_IMPL(AST_NAME) void AST_NAME##Node::visit(CodeGen *cg, CodePack *cp) { \
		cg->visit(cp, this); \
	}

	NODE_LIST(VIRTUAL_IMPL)

	/*
	void NumberNode::visit(CodeGen *cg, CodePack* cp)
	{

	}
	*/

	/*
	void Node::visit(CodeGen *cg, CodePack* cp)
	{
		cg->visit(cp, this);
	}
	*/
};