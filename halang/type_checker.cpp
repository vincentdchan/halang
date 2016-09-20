#include "stdafx.h"
#include "type_checker.h"
#include <memory>

namespace halang
{
	typedef std::unique_ptr<TypeChecker> PTC;

	TypeChecker::TypeChecker() : 
		guestFuncType(false), env(nullptr)
	{
		env = new TypeCheckEnv();
	}

	TypeChecker* TypeChecker::TypeCheck(Node *node)
	{
		auto tc = new TypeChecker();
		tc->visit(node);
		return tc;
	}

	void TypeChecker::visit(BlockExprNode* node)
	{
		for (auto i = node->children.begin();
			i != node->children.end(); ++i)
			this->visit(*i);
	}

	void TypeChecker::visit(NumberNode* node)
	{
		if (node->maybeInt)
			node->typeInfo.setType(Type::INT);
		else
			node->typeInfo.setType(Type::NUMBER);
	}

	void TypeChecker::visit(StringNode* node)
	{
		node->typeInfo.setType(Type::STRING);
	}

	void TypeChecker::visit(UnaryExprNode* node)
	{
		visit(node->child);

		switch (node->op)
		{
		case OperatorType::ADD:
		case OperatorType::SUB:
			if (node->child->typeInfo.type == Type::INT ||
				node->child->typeInfo.type == Type::NUMBER)
				node->typeInfo = node->child->typeInfo;
			else
				ReportError("Use numberic operation on non-number value.");
			break;
		case OperatorType::NOT:
			if (node->child->typeInfo.type == Type::BOOL)
				node->typeInfo.setType(Type::BOOL);
			break;
		default:
			ReportError("Do not use correct operation on unary expression.");
		}
	}

	void TypeChecker::visit(BinaryExprNode* node)
	{
		visit(node->right);
		visit(node->left);

		switch (node->op)
		{
		case OperatorType::MOD:
		{
			if (node->left->typeInfo.type == Type::INT &&
				node->right->typeInfo.type == Type::INT)
				node->typeInfo.setType(Type::INT);
			else
				ReportError("Use mod operation on non-integer value.");
		}
		case OperatorType::ADD:
		case OperatorType::SUB:
		case OperatorType::MUL:
		case OperatorType::DIV:
		case OperatorType::POW:
		{
			if (node->left->typeInfo.type == Type::INT &&
				node->right->typeInfo.type == Type::INT)
				node->typeInfo.setType(Type::INT);
			else if (node->left->typeInfo.type == Type::NUMBER ||
				node->right->typeInfo.type == Type::NUMBER)
				node->typeInfo.setType(Type::NUMBER);
			break;
		}
		case OperatorType::EQ:
		case OperatorType::GT:
		case OperatorType::GTEQ:
		case OperatorType::LT:
		case OperatorType::LTEQ:
		{
			if (node->left->typeInfo.type == Type::BOOL &&
				node->right->typeInfo.type == Type::BOOL)
				node->typeInfo.setType(Type::BOOL);
			else
				ReportError("Use bool operation on non-boolean value.");
			break;
		}
		default:
			ReportError("Unkown binary operation.");
		}

	}

	void TypeChecker::visit(VarStmtNode* node)
	{
		for (auto i = node->children.begin();
			i != node->children.end(); ++i)
			visit(*i);
	}

	void TypeChecker::visit(VarSubExprNode* node)
	{
		if (node->typeName)
		{
			auto tName = node->typeName->name;
			node->typeInfo = getTypeFromString(tName);

			if (node->expression)
			{
				visit(node->expression);
				if (node->expression->typeInfo != node->typeInfo)
					ReportError("The variable statement's type isn't match the defined one");
			}
		}
		else if (node->expression)
		{
			visit(node->expression);
			node->typeInfo = node->expression->typeInfo;
		}
	}

	void TypeChecker::visit(AssignmentNode* node)
	{
		auto typeName = node->identifier->name;
		for (auto i = env->vars.begin(); i != env->vars.end(); ++i)
		{
			if (i->first == typeName)
			{
				visit(node->expression);
				if (node->expression->typeInfo == i->second)
					node->typeInfo = i->second;
				else
					ReportError("Assignment type doesn't match.");
				return;
			}
		}

		ReportError("identifier not found in variable table.");
	}

	/// <summary>
	/// Find the name of identifier in the variable table or type table.
	/// Report error if the identifier not found.
	/// </summary>
	void TypeChecker::visit(IdentifierNode* node)
	{
		for (auto i = env->vars.begin(); i != env->vars.end(); ++i)
		{
			if (i->first == node->name)
			{
				node->typeInfo = i->second;
				return;
			}
		}

		ReportError("identifier not found in variable table and type table.");
	}

	void TypeChecker::visit(IfStmtNode* node)
	{
		visit(node->condition);
		if (node->condition->typeInfo.type != Type::BOOL)
		{
			ReportError("The condition of if-else statement'type MUST be bool.");
			return;
		}
		visit(node->true_branch);
		visit(node->false_branch);
	}

	void TypeChecker::visit(WhileStmtNode* node)
	{
		visit(node->condition);
		if (node->condition->typeInfo.type != Type::BOOL)
		{
			ReportError("The condition of if-else statement'type MUST be bool.");
			return;
		}
		visit(node->child);
	}

	void TypeChecker::visit(BreakStmtNode* node)
	{

	}

	void TypeChecker::visit(FuncDefNode* node)
	{
		Type _type;
		if (node->typeName)
		{
			auto typeName = node->typeName->name;
			_type = getTypeFromString(typeName);
			guestFuncType = false;
			mustReturnType = _type;
		}
		else
		{
			guestFuncType = true;
			guestType.release();
		}

		for (auto i = node->parameters.begin();
			i != node->parameters.end(); ++i)
		{
			visit(*i);
		}

		auto new_env = new TypeCheckEnv();
		new_env->prev = env;
		env = new_env;

		visit(node->block);

		env = env->prev;
		delete new_env;

		node->typeInfo = *guestType;
	}

	void TypeChecker::visit(FuncDefParamNode* node)
	{
		node->typeInfo = getTypeFromString(node->typeName);
	}

	void TypeChecker::visit(FuncCallNode* node)
	{
		for (auto i = node->parameters.begin();
			i != node->parameters.end(); ++i)
			visit(*i);

		// find the function definition and compare to the parameters
	}

	void TypeChecker::visit(ReturnStmtNode* node)
	{
		visit(node->expression);

		if (guestFuncType)
		{
			if (guestType == nullptr)
				guestType = std::make_unique<Type>(currentType);
			else
			{
				if (*guestType != currentType)
				{
					ReportError("A function have different return type.");
				}
			}
		}
		else
		{
			if (currentType != mustReturnType)
			{
				ReportError("Type not match.");
			}
		}

	}

	void TypeChecker::visit(PrintStmtNode* node)
	{
		visit(node->expression);
		if (node->expression->typeInfo.type != Type::STRING)
			ReportError("The print statement MUST be followed by STRING.");
	}

	Type TypeChecker::getTypeFromString(std::string typeName)
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