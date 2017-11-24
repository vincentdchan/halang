#include "ast.h"
#include "visitor.h"
#include "codegen.h"

namespace halang
{

#define VIRTUAL_IMPL(AST_NAME) void AST_NAME##Node::visit(Visitor *visitor) { \
		visitor->visit(this); \
	}

	NODE_LIST(VIRTUAL_IMPL)

#undef VIRTUAL_IMPL

};
