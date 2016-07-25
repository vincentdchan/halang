#pragma once
#include <cinttypes>
#include <vector>
#include "token.h"

#define NODE_LIST(V) \
	V(Identifier) \
	V(Assignment) \
	V(Number) \
	V(BinaryExpr) \
	V(UnaryExpr) \
	V(BlockExpr) \
	V(IfStmt) \
	V(WhileStmt) \
	V(FunctionStmt) \
	V(FunctionParameters) \

namespace parser
{
	class Node;
#define E(Name) class Name##Node;
	NODE_LIST(E)
#undef E


	class Node
	{
	public:
		Node() {}
		virtual NumberNode* asNumber() { return nullptr; }
		virtual IdentifierNode* asIdentifier() { return nullptr; }
		virtual AssignmentNode* asAssignment() { return nullptr; }
		virtual UnaryExprNode* asUnaryExpression() { return nullptr; }
		virtual BlockExprNode* asBlockExpression() { return nullptr; }
		virtual BinaryExprNode* asBinaryExpression() { return nullptr; }
		virtual IfStmtNode* asIfStmt() { return nullptr; }
		virtual WhileStmtNode* asWhileStmt() { return nullptr; }
		virtual FunctionStmtNode* asFunctionStmt() { return nullptr; }
		virtual FunctionParametersNode* asFunctionParameters() { return nullptr; }
		virtual ~Node() {}
	};

	class IdentifierNode : public Node
	{
	public:
		IdentifierNode(const std::string& _str) : name(_str)
		{}
		virtual IdentifierNode* asIdentifier() override { return this; }

		std::string name;
	};

	class AssignmentNode : public Node
	{
	public:
		AssignmentNode() {}
		AssignmentNode(const std::string& _id, Node* _exp = nullptr):
			identifier(_id), expression(_exp)
		{}
		virtual AssignmentNode* asAssignment() override { return this; }

		std::string identifier;
		Node* expression;
	};

	class BlockExprNode : public Node
	{
	public:
		BlockExprNode() {}
		std::vector<Node*> children;
		virtual BlockExprNode* asBlockExpression() override { return this; }
	};

	class UnaryExprNode : public Node
	{
	public:
		UnaryExprNode(OperatorType _op = OperatorType::ILLEGAL_OP, 
			Node* _after = nullptr):
			op(_op), child(_after)
		{}
		virtual UnaryExprNode* asUnaryExpression() override { return this; }
		OperatorType op;
		Node* child;
	};

	class BinaryExprNode : public Node
	{
	public:
		BinaryExprNode(OperatorType _op = OperatorType::ILLEGAL_OP, 
			Node* _left = nullptr, 
			Node* _right = nullptr):
			op(_op), left(_left), right(_right)
		{}
		virtual BinaryExprNode* asBinaryExpression() override { return this; }

		OperatorType op;
		Node* left;
		Node* right;
	};

	class NumberNode : public Node
	{
	public:
		NumberNode(double _decimal = 0.0, bool _mi = false):
			value(_decimal), maybeInt(_mi)
		{}
		virtual NumberNode* asNumber() override { return this; }

		double value;
		bool maybeInt;
	};

	class IfStmtNode : public Node
	{
	public:
		IfStmtNode(Node* _cond = nullptr, 
			Node* _true = nullptr, 
			Node* _false = nullptr):
			condition(_cond), 
			true_branch(_true), 
			false_branch(_false)
		{}
 		virtual IfStmtNode* asIfStmt() override { return this; }

		Node* condition;
		Node* true_branch;
		Node* false_branch;
	};

	class WhileStmtNode : public Node
	{
	public:
		WhileStmtNode(Node* _con = nullptr, 
			Node* _cont = nullptr) :
			condition(_con), child(_cont)
		{}
 		virtual WhileStmtNode* asWhileStmt() override { return this; }

		Node* condition;
		Node* child;
	};

	class FunctionStmtNode : public Node
	{
	public:
		FunctionStmtNode() {}
		virtual FunctionStmtNode* asFunctionStmt() override { return this; }
		std::string name;
		FunctionParametersNode* parameters;
		BlockExprNode* block;
	};

	class FunctionParametersNode : public Node
	{
	public:
		virtual FunctionParametersNode* asFunctionParameters() override { return this; }
		std::vector<std::string> identifiers;
	};

}
