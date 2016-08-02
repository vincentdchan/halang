#pragma once
#include <cinttypes>
#include <vector>
#include "token.h"

namespace halang
{

#define NODE_LIST(V) \
	V(Assignment) \
	V(Number) \
	V(Identifier) \
	V(BinaryExpr) \
	V(UnaryExpr) \
	V(BlockExpr) \
	V(VarStmt) \
	V(IfStmt) \
	V(WhileStmt) \
	V(BreakStmt) \
	V(ReturnStmt) \
	V(PrintStmt) \
	V(FuncDef) \
	V(FuncDefParams) \
	V(FuncCall) \
	V(FuncCallParams)

	class CodePack;
	class CodeGen;

	class Node;
#define E(Name) class Name##Node;
	NODE_LIST(E)
#undef E

#define VISIT_OVERRIDE virtual void visit(CodeGen *cg, CodePack* cp) override;


	class Node
	{
	public:
		Node() {}
		virtual IdentifierNode* asIdentifier() { return nullptr; }
		virtual NumberNode* asNumber() { return nullptr; }
		virtual AssignmentNode* asAssignment() { return nullptr; }
		virtual UnaryExprNode* asUnaryExpression() { return nullptr; }
		virtual BlockExprNode* asBlockExpression() { return nullptr; }
		virtual BinaryExprNode* asBinaryExpression() { return nullptr; }
		virtual VarStmtNode* asVarStmt() { return nullptr; }
		virtual IfStmtNode* asIfStmt() { return nullptr; }
		virtual WhileStmtNode* asWhileStmt() { return nullptr; }
		virtual BreakStmtNode* asBreakStmt() { return nullptr; }
		virtual ReturnStmtNode* asReturnStmt() { return nullptr; }
		virtual FuncDefNode* asFuncDef() { return nullptr; }
		virtual FuncDefParamsNode* asFuncDefParams() { return nullptr; }
		virtual FuncCallNode* asFuncCall() { return nullptr; }
		virtual FuncCallParamsNode* asFuncCallParams() { return nullptr; }
		virtual PrintStmtNode* asPrintStmt() { return nullptr; }

		virtual void visit(CodeGen *cg, CodePack* cp) = 0;
			/*
		{
			cg->visit(cp, this);
		}
		*/
	};

	class NumberNode : public Node
	{
	public:
		NumberNode(double _num = 0, bool _mi = false) :
			number(_num), maybeInt(_mi)
		{}
		virtual NumberNode* asNumber() override { return nullptr; }
		double number;
		bool maybeInt;

		VISIT_OVERRIDE
	};

	class IdentifierNode : public Node
	{
	public:
		IdentifierNode(const std::string& _str) : name(_str)
		{}
		virtual IdentifierNode* asIdentifier() override { return this; }

		std::string name;

		VISIT_OVERRIDE
	};

	class AssignmentNode : public Node
	{
	public:
		AssignmentNode() {}
		AssignmentNode(IdentifierNode* _id, Node* _exp = nullptr) :
			identifier(_id), expression(_exp)
		{}
		virtual AssignmentNode* asAssignment() override { return this; }

		IdentifierNode* identifier;
		Node* expression;

		VISIT_OVERRIDE
	};

	class BlockExprNode : public Node
	{
	public:
		BlockExprNode() {}
		std::vector<Node*> children;
		virtual BlockExprNode* asBlockExpression() override { return this; }

		VISIT_OVERRIDE
	};

	class UnaryExprNode : public Node
	{
	public:
		UnaryExprNode(OperatorType _op = OperatorType::ILLEGAL_OP,
			Node* _after = nullptr) :
			op(_op), child(_after)
		{}
		virtual UnaryExprNode* asUnaryExpression() override { return this; }
		OperatorType op;
		Node* child;

		VISIT_OVERRIDE
	};

	class BinaryExprNode : public Node
	{
	public:
		BinaryExprNode(OperatorType _op = OperatorType::ILLEGAL_OP,
			Node* _left = nullptr,
			Node* _right = nullptr) :
			op(_op), left(_left), right(_right)
		{}
		virtual BinaryExprNode* asBinaryExpression() override { return this; }

		OperatorType op;
		Node* left;
		Node* right;

		VISIT_OVERRIDE
	};


	class VarStmtNode : public Node
	{
	public:
		virtual VarStmtNode* asVarStmt() override { return this; }
		std::vector<Node*> children;

		VISIT_OVERRIDE
	};

	class IfStmtNode : public Node
	{
	public:
		IfStmtNode(Node* _cond = nullptr,
			Node* _true = nullptr,
			Node* _false = nullptr) :
			condition(_cond),
			true_branch(_true),
			false_branch(_false)
		{}
		virtual IfStmtNode* asIfStmt() override { return this; }

		Node* condition;
		Node* true_branch;
		Node* false_branch;

		VISIT_OVERRIDE
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

		VISIT_OVERRIDE
	};

	class BreakStmtNode : public Node
	{
	public:
		virtual BreakStmtNode* asBreakStmt() override { return this; }

		VISIT_OVERRIDE
	};

	class ReturnStmtNode : public Node
	{
	public:
		ReturnStmtNode(Node * exp = nullptr) : expression(exp)
		{}
		virtual ReturnStmtNode* asReturnStmt() override { return this; }
		Node* expression;

		VISIT_OVERRIDE
	};

	class FuncDefNode : public Node
	{
	public:
		FuncDefNode() {}
		virtual FuncDefNode* asFuncDef() override { return this; }
		IdentifierNode* name;
		FuncDefParamsNode* parameters;
		BlockExprNode* block;

		VISIT_OVERRIDE
	};

	class FuncDefParamsNode : public Node
	{
	public:
		virtual FuncDefParamsNode* asFuncDefParams() override { return this; }
		std::vector<std::string> identifiers;

		VISIT_OVERRIDE
	};

	class FuncCallNode : public Node
	{
	public:
		FuncCallNode(Node* _exp = nullptr, FuncCallParamsNode* _params = nullptr) :
			exp(_exp), params(_params)
		{}
		virtual FuncCallNode* asFuncCall() override { return this; }
		Node* exp; // maybe identifier, maybe another func  foo(a) foo(a)(b)(b)
		FuncCallParamsNode* params;

		VISIT_OVERRIDE
	};

	class FuncCallParamsNode : public Node
	{
	public:
		virtual FuncCallParamsNode* asFuncCallParams() override { return this; }
		std::vector<Node*> children;

		VISIT_OVERRIDE
	};

	class PrintStmtNode : public Node
	{
	public:
		PrintStmtNode(Node * exp = nullptr) : expression(exp)
		{}
		virtual PrintStmtNode* asPrintStmt() override { return this; }
		Node* expression;

		VISIT_OVERRIDE
	};

};
