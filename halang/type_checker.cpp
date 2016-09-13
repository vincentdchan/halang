#include "stdafx.h"
#include "type_checker.h"
#include <memory>

namespace halang
{
	std::unique_ptr<TypeChecker> TypeChecker::TypeCheck(Node* node)
	{
		auto ptr = std::make_unique<TypeChecker>();
		return ptr;
	}
}