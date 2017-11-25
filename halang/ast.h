#pragma once
#include <cinttypes>
#include <vector>
#include <memory>
#include <string>
#include "token.h"

namespace halang
{

#define NODE_LIST(V) \
	V(Program) \
	V(Number) \
	V(Identifier) \
	V(String) \
	V(LetStatement) \
	V(IfStatement) \
	V(WhileStatement) \
	V(BreakStatement) \
	V(ContinueStatement) \
	V(ReturnStatement) \
	V(DefStatement) \
	V(MemberExpression) \
	V(CallExpression) \
	V(AssignExpression) \
	V(ListExpression) \
	V(UnaryExpression) \
	V(BinaryExpression) \
	V(DoExpression) \
	V(FunExpression) \

	class CodePack;
	class CodeGen;
	class Visitor;

	class Node;
#define E(Name) class Name##Node;
	NODE_LIST(E)
#undef E

#define VISIT_OVERRIDE virtual void visit(Visitor*) override;


	/// <summary>
	/// A node that is the basic class of all ast node,
	/// providing visiter for codegen and store the type infomation.
	/// 
	/// The sub class MUST override the virtual function for the class
	/// </summary>
	class Node
	{
	public:
		Node() {}

		Location begin_location;
		Location end_location;

		virtual ProgramNode* AsProgram() { return nullptr; }
		virtual StringNode* AsString() { return nullptr; }
		virtual IdentifierNode* AsIdentifier() { return nullptr; }
		virtual NumberNode* AsNumber() { return nullptr; }
		virtual LetStatementNode* AsLetStatement() { return nullptr; }
		virtual IfStatementNode* AsIfStatement() { return nullptr; }
		virtual WhileStatementNode* AsWhileStatement() { return nullptr; }
		virtual BreakStatementNode* AsBreakStatement() { return nullptr; }
		virtual ContinueStatementNode* AsContinueStatement() { return nullptr; }
		virtual ReturnStatementNode* AsReturnStatement() { return nullptr; }
		virtual MemberExpressionNode* AsMemberExpression() { return nullptr; }
		virtual DefStatementNode* AsDefStatement() { return nullptr; }
		virtual CallExpressionNode* AsCallExpression() { return nullptr; }
		virtual AssignExpressionNode* AsAssignExpression() { return nullptr; }
		virtual ListExpressionNode* AsListExpression() { return nullptr; }
		virtual UnaryExpressionNode* AsUnaryExpression() { return nullptr; }
		virtual BinaryExpressionNode* AsBinaryExpression() { return nullptr; }
		virtual DoExpressionNode* AsDoExpression() { return nullptr; }
		virtual FunExpressionNode* AsFunExpression() { return nullptr; }

		virtual void visit(Visitor*) = 0;
	};

	class ProgramNode : public Node {
	public:

		virtual ProgramNode* 
		AsProgram() override { 
			return this; 
		}

		std::vector<Node*> statements;

		VISIT_OVERRIDE
	};

	class StringNode : public Node
	{
	public:
		StringNode() {
		}

		StringNode(const std::u16string& _content) : content(_content)
		{}

		virtual StringNode* AsString() override { return this; }
		std::u16string content;

		VISIT_OVERRIDE
	};

	class NumberNode : public Node
	{
	public:
		NumberNode(double _num = 0, bool _mi = false) :
			number(_num), maybeInt(_mi)
		{ }
		virtual NumberNode* AsNumber() override { return this; }
		double number;
		bool maybeInt;

		VISIT_OVERRIDE
	};

	/// <summary>
	/// identifier
	/// </summary>
	class IdentifierNode : public Node
	{
	public:
		IdentifierNode() { }
		IdentifierNode(const U16String& _str) : name(_str)
		{ }
		virtual IdentifierNode* AsIdentifier() override { return this; }

		U16String name;

		VISIT_OVERRIDE
	};

	class LetStatementNode : public Node {
	public:

		std::vector<Node*> assignments;

		virtual 
		LetStatementNode* AsLetStatement() override {
			return this;
		}

		VISIT_OVERRIDE
	};

	class IfStatementNode : public Node
	{
	public:
		virtual IfStatementNode* AsIfStatement() override { return this; }

		Node* condition = nullptr;

		std::vector<Node*> children;
		std::vector<Node*> else_children;

		VISIT_OVERRIDE
	};

	class WhileStatementNode : public Node
	{
	public:

		virtual 
		WhileStatementNode* AsWhileStatement() override { 
			return this; 
		}

		Node* condition = nullptr;

		std::vector<Node*> children;

		VISIT_OVERRIDE
	};

	class BreakStatementNode : public Node
	{
	public:
		virtual 
		BreakStatementNode* 
		AsBreakStatement() override { 
			return this; 
		}

		VISIT_OVERRIDE
	};

	class ContinueStatementNode : public Node
	{
	public:
		virtual 
		ContinueStatementNode* 
		AsContinueStatement() override { 
			return this; 
		}

		VISIT_OVERRIDE
	};

	class ReturnStatementNode : public Node
	{
	public:
		ReturnStatementNode(Node * exp = nullptr) : expression(exp)
		{}

		virtual 
		ReturnStatementNode* 
		AsReturnStatement() override { 
			return this; 
		}

		Node* expression = nullptr;

		VISIT_OVERRIDE
	};

	class DefStatementNode : public Node
	{
	public:

		virtual DefStatementNode* 
		AsDefStatement() override { 
			return this; 
		}

		IdentifierNode* name = nullptr;
		std::vector<Node*> params;
		std::vector<Node*> body;

		VISIT_OVERRIDE
	};

	class MemberExpressionNode : public Node
	{
	public:

		virtual MemberExpressionNode* 
		AsMemberExpression() override { 
			return this; 
		}

		Node* left = nullptr;
		Node* right = nullptr;

		VISIT_OVERRIDE
	};

	class CallExpressionNode : public Node
	{
	public:

		virtual CallExpressionNode* 
		AsCallExpression() override { 
			return this; 
		}

		Node* callee = nullptr;
		std::vector<Node*> parameters;

		VISIT_OVERRIDE
	};

	class AssignExpressionNode : public Node
	{
	public:

		virtual AssignExpressionNode* 
		AsAssignExpression() override { 
			return this; 
		}

		IdentifierNode* identifier = nullptr;
		Node* expression = nullptr;

		VISIT_OVERRIDE
	};

	class ListExpressionNode : public Node
	{
	public:

		std::vector<Node*> children;

		virtual ListExpressionNode* 
		AsListExpression() override { 
			return this; 
		}

		VISIT_OVERRIDE
	};

	class UnaryExpressionNode : public Node
	{
	public:

		virtual UnaryExpressionNode* 
		AsUnaryExpression() override { return this; }

		OperatorType op = OperatorType::ILLEGAL_OP;
		Node* child = nullptr;;

		VISIT_OVERRIDE
	};

	class BinaryExpressionNode : public Node
	{
	public:

		virtual BinaryExpressionNode* 
		AsBinaryExpression() override { return this; }

		OperatorType op;
		Node* left = nullptr;
		Node* right = nullptr;

		VISIT_OVERRIDE
	};

	class DoExpressionNode : public Node {
	public:

		virtual DoExpressionNode*
		AsDoExpression() override {
			return this;
		}

		std::vector<Node*> children;

		VISIT_OVERRIDE
	};

	class FunExpressionNode : public Node
	{
	public:

		virtual FunExpressionNode* 
		AsFunExpression() override { 
			return this;
		}

		std::vector<Node*> params;
		Node* expression = nullptr;

		VISIT_OVERRIDE
	};

};
