#include "stdafx.h"
#include "type_checker.h"
#include <memory>

namespace halang
{
	typedef std::unique_ptr<TypeChecker> PTC;

	PTC TypeChecker::TypeCheck(Node* node)
	{
		auto ptr = make_unique<TypeChecker>();
		TypeCheck(*ptr, node);
		return ptr;
	}

	void TypeChecker::TypeCheck(TypeChecker& tc, Node* node)
	{
		if (node->asFuncDef())
		{
			TypeCheck(tc, node->asFuncDef());
		}
		else if (node->asReturnStmt())
		{
			TypeCheck(tc, node->asReturnStmt());
		}
		else if (node->asFuncCall())
		{
			TypeCheck(tc, node->asFuncCall());
		}
	}

	void TypeChecker::TypeCheck(TypeChecker& tc, NumberNode* node)
	{
		if (node->maybeInt)
			node->typeInfo.reset(new Type(Type::INT));
		else
			node->typeInfo.reset(new Type(Type::NUMBER));
	}

	void TypeChecker::TypeCheck(TypeChecker& tc, UnaryExprNode* node)
	{
		TypeCheck(tc, node->child);
		switch (node->op)
		{
		case OperatorType::ADD:
		case OperatorType::SUB:
			if (node->child->typeInfo->type == Type::INT ||
				node->child->typeInfo->type == Type::NUMBER)
			{
				node->typeInfo.reset(new Type(*node->child->typeInfo));
			}
			else
				tc.ReportError("Use numberic operation on non-number value.");
			break;
		case OperatorType::NOT:
			if (node->child->typeInfo->type == Type::BOOL)
				node->typeInfo.reset(new Type(Type::BOOL));
			break;
		default:
			tc.ReportError("Do not use correct operation on unary expression.");
		}
	}

	void TypeChecker::TypeCheck(TypeChecker& tc, BinaryExprNode* node)
	{
		TypeCheck(tc, node->right);
		TypeCheck(tc, node->left);

		switch (node->op)
		{
		case OperatorType::MOD:
		{
			if (node->left->typeInfo->type == Type::INT &&
				node->right->typeInfo->type == Type::INT)
				node->typeInfo.reset(new Type(Type::INT));
			else
				tc.ReportError("Use mod operation on non-integer value.");
		}
		case OperatorType::ADD:
		case OperatorType::SUB:
		case OperatorType::MUL:
		case OperatorType::DIV:
		case OperatorType::POW:
		{
			if (node->left->typeInfo->type == Type::INT &&
				node->right->typeInfo->type == Type::INT)
				node->typeInfo.reset(new Type(Type::INT));
			else if (node->left->typeInfo->type == Type::NUMBER ||
				node->right->typeInfo->type == Type::NUMBER)
				node->typeInfo.reset(new Type(Type::NUMBER));
			break;
		}
		case OperatorType::EQ:
		case OperatorType::GT:
		case OperatorType::GTEQ:
		case OperatorType::LT:
		case OperatorType::LTEQ:
		{
			if (node->left->typeInfo->type == Type::BOOL &&
				node->right->typeInfo->type == Type::BOOL)
				node->typeInfo.reset(new Type(Type::BOOL));
			else
				tc.ReportError("Use bool operation on non-boolean value.");
			break;
		}
		default:
			tc.ReportError("Unkown binary operation.");
		}

	}

	void TypeChecker::TypeCheck(TypeChecker& tc, VarStmtNode* node)
	{

	}

	void TypeChecker::TypeCheck(TypeChecker& tc, VarInitExprNode* node)
	{

	}

	void TypeChecker::TypeCheck(TypeChecker& tc, IdentifierNode* node)
	{
		for (auto i = tc.env->vars.begin(); i != tc.env->vars.end(); ++i)
		{
			if (i->first == node->name)
			{
				node->typeInfo.reset(new Type(i->second));
				break;
			}
		}
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

		*(node->typeInfo) = *tc.guestType;
	}

	void TypeChecker::TypeCheck(TypeChecker& tc, FuncCallNode* node)
	{
		for (auto i = node->parameters.begin();
			i != node->parameters.end(); ++i)
			TypeCheck(tc, *i);

		// find the function definition and compare to the parameters
	}

	void TypeChecker::TypeCheck(TypeChecker& tc, BlockExprNode* node)
	{
		for (auto i = node->children.begin(); i != node->children.end(); ++i)
			TypeCheck(tc, *i);

		// a typeInfo of a Block is equal to the last statement of the block
		node->typeInfo.reset(new Type(*node->children[node->children.size() - 1]->typeInfo));
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