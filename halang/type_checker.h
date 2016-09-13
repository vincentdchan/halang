#pragma once
#include "ast.h"
#include <memory>

namespace halang
{

	class TypeChecker
	{
		static std::unique_ptr<TypeChecker> TypeCheck(Node* node);

	};
}
