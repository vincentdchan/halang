#include "ast.h"
#include "visitor.h"
#include "codegen.h"

namespace halang
{

#define VIRTUAL_IMPL(AST_NAME) void AST_NAME##Node::Visit(Visitor *visitor) { \
		visitor->Visit(this); \
	}

	NODE_LIST(VIRTUAL_IMPL)

#undef VIRTUAL_IMPL

};
