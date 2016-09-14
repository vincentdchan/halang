#pragma once
#include "ast.h"
#include "util.h"
#include "type.h"
#include <memory>

namespace halang
{
	class TypeChecker : public utils::_MessageContainer
	{
	public:
		TypeChecker() : guestFuncType(false)
		{

		}

		static std::unique_ptr<TypeChecker> TypeCheck(Node* node);

	private:

		static void TypeCheck(TypeChecker& tc, Node* node);
		static void TypeCheck(TypeChecker& tc, FuncDefNode* node);
		static void TypeCheck(TypeChecker& tc, BlockExprNode* node);
		static void TypeCheck(TypeChecker& tc, FuncCallNode* node);
		static void TypeCheck(TypeChecker& tc, ReturnStmtNode* node);

		static Type getTypeFromString(TypeChecker* tc, std::string typeName);

		Type mustReturnType;
		Type currentType;
		std::unique_ptr<Type> guestType;

		bool guestFuncType;
	};
}
