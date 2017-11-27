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

		virtual void Visit(Node* node) = 0;
#define E(Name) virtual void Visit(Name##Node*) = 0;
		NODE_LIST(E)
#undef E
		virtual ~Visitor() {}

		inline void IncreDepth() {
			depth++;
		}

		inline void DecreDepth() {
			depth--;
		}

	protected:

		int depth;

	};

}
