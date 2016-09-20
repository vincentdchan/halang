#pragma once
#include "ast.h"
#define E(Name) class Name##Node;
	NODE_LIST(E)
#undef E

namespace halang
{

	class Visitor
	{
	public:

		virtual void visit(Node* node) = 0;
#define E(Name) virtual void visit(Name##Node*) = 0;
		NODE_LIST(E)
#undef E
		virtual ~Visitor() {}

	};

}
