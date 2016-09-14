#include "stdafx.h"
#include "type_checker.h"
#include <memory>

namespace halang
{
	typedef std::unique_ptr<TypeChecker> PTC;

	PTC TypeChecker::TypeCheck(Node* node)
	{
		auto ptr = make_unique<TypeChecker>();
		if (node->asFuncDef())
		{
			TypeCheck(*ptr, node->asFuncDef());
		}
		else if (node->asReturnStmt())
		{
			TypeCheck(*ptr, node->asReturnStmt());
		}
		else if (node->asFuncCall())
		{
			TypeCheck(*ptr, node->asFuncCall());
		}
		return ptr;
	}

	void TypeChecker::TypeCheck(TypeChecker& tc, Node* node)
	{
	}

	void TypeChecker::TypeCheck(TypeChecker& tc, FuncDefNode* node)
	{
		Type _type;
		if (node->typeName)
		{
			auto typeName = node->typeName->name;
			_type = getTypeFromString(&tc, typeName);
			tc.guestFuncType = false;
			tc.mustReturnType = _type;
		}
		else
		{
			tc.guestFuncType = true;
			tc.guestType.release();
		}
		TypeCheck(tc, node->block);
		node->type = *tc.guestType;
	}

	void TypeChecker::TypeCheck(TypeChecker& tc, FuncCallNode* node)
	{

	}

	void TypeChecker::TypeCheck(TypeChecker& tc, BlockExprNode* node)
	{
		for (int i = 0; i < node->children.size(); ++i)
			TypeCheck(tc, node->children[i]);
	}

	void TypeChecker::TypeCheck(TypeChecker& tc, ReturnStmtNode* node)
	{
		TypeCheck(tc, node->expression);

		if (tc.guestFuncType)
		{
			if (tc.guestType == nullptr)
				tc.guestType = std::make_unique<Type>(tc.currentType);
			else
			{
				if (*tc.guestType != tc.currentType)
				{
					tc.ReportError("A function have different return type.");
				}
			}
		}
		else
		{
			if (tc.currentType != tc.mustReturnType)
			{
				tc.ReportError("Type not match.");
			}
		}

	}

	Type TypeChecker::getTypeFromString(TypeChecker* tc, std::string typeName)
	{
		Type t;
		if (typeName == "int")
			t.type = Type::INT;
		else if (typeName == "number")
			t.type = Type::NUMBER;
		else if (typeName == "bool")
			t.type = Type::BOOL;
		return t;
	}

}