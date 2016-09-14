#pragma once
#include <cinttypes>
#include <vector>
#include "token.h"
#include "string.h"
#include "type.h"

namespace halang
{

#define NODE_LIST(V) \
	V(Assignment) \
	V(Number) \
	V(Identifier) \
	V(String) \
	V(BinaryExpr) \
	V(UnaryExpr) \
	V(BlockExpr) \
	V(VarStmt) \
	V(VarInitExpr) \
	V(IfStmt) \
	V(WhileStmt) \
	V(BreakStmt) \
	V(ReturnStmt) \
	V(PrintStmt) \
	V(FuncDef) \
	V(FuncDefParam)\
	V(FuncCall) \
	V(FuncCallParam) \

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
		virtual StringNode* asString() { return nullptr; }
		virtual IdentifierNode* asIdentifier() { return nullptr; }
		virtual NumberNode* asNumber() { return nullptr; }
		virtual AssignmentNode* asAssignment() { return nullptr; }
		virtual UnaryExprNode* asUnaryExpression() { return nullptr; }
		virtual BlockExprNode* asBlockExpression() { return nullptr; }
		virtual BinaryExprNode* asBinaryExpression() { return nullptr; }
		virtual VarStmtNode* asVarStmt() { return nullptr; }
		virtual VarInitExprNode* asVarInitExpr() { return nullptr; }
		virtual IfStmtNode* asIfStmt() { return nullptr; }
		virtual WhileStmtNode* asWhileStmt() { return nullptr; }
		virtual BreakStmtNode* asBreakStmt() { return nullptr; }
		virtual ReturnStmtNode* asReturnStmt() { return nullptr; }
		virtual FuncDefNode* asFuncDef() { return nullptr; }
		virtual FuncDefParamNode* asFuncDefParam() { return nullptr; }
		virtual FuncCallNode* asFuncCall() { return nullptr; }
		virtual FuncCallParamNode* asFuncCallParam() { return nullptr; }
		virtual FuncCallParamsNode* asFuncCallParams() { return nullptr; }
		virtual PrintStmtNode* asPrintStmt() { return nullptr; }

		virtual void visit(CodeGen *cg, CodePack* cp) = 0;
	};

	class StringNode : public Node
	{
	public:
		StringNode(IString _content) : content(_content)
		{}

		virtual StringNode* asString() override { return this; }
		IString content;

		VISIT_OVERRIDE
	};

	class NumberNode : public Node
	{
	public:
		NumberNode(double _num = 0, bool _mi = false) :
			number(_num), maybeInt(_mi)
		{ }
		virtual NumberNode* asNumber() override { return this; }
		double number;
		bool maybeInt;

		Type type;
		VISIT_OVERRIDE
	};

	class IdentifierNode : public Node
	{
	public:
		IdentifierNode(const std::string& _str) : name(_str)
		{}
		virtual IdentifierNode* asIdentifier() override { return this; }

		std::string name;

		Type type;
		VISIT_OVERRIDE
	};

	class AssignmentNode : public Node
	{
	public:
		AssignmentNode() {}
		AssignmentNode(IdentifierNode* _id, Node* _exp = nullptr, IdentifierNode* _typeName = nullptr) :
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
		std::vector<VarInitExprNode*> children;

		VISIT_OVERRIDE
	};

	class VarInitExprNode : public Node
	{
	public:
		VarInitExprNode() :
			varName(nullptr), typeName(nullptr), expression(nullptr)
		{}
		virtual VarInitExprNode* asVarInitExpr() override { return this; }
		IdentifierNode* varName;
		IdentifierNode* typeName;
		Node* expression;

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
		std::vector<FuncDefParamNode*> parameters;

		BlockExprNode* block;
		IdentifierNode* typeName;

		Type type;
		VISIT_OVERRIDE
	};

	/*
	class FuncDefParamsNode : public Node
	{
	public:
		virtual FuncDefParamsNode* asFuncDefParams() override { return this; }
		std::vector<FuncDefParamNode> defs;

		VISIT_OVERRIDE
	};
	*/

	class FuncDefParamNode : public Node
	{
	public:
		FuncDefParamNode()
		{}
		virtual FuncDefParamNode* asFuncDefParam() override { return this; }
		std::string name;
		std::string typeName;

		VISIT_OVERRIDE
	};

	class FuncCallNode : public Node
	{
	public:
		FuncCallNode(Node* _exp = nullptr) :
			exp(_exp)
		{}
		virtual FuncCallNode* asFuncCall() override { return this; }
		Node* exp; // maybe identifier, maybe another func  foo(a) foo(a)(b)(b)
		// FuncCallParamsNode* params;
		std::vector<FuncCallParamNode*> parameters;

		Type type;
		VISIT_OVERRIDE
	};

	class FuncCallParamNode : public Node
	{
	public:
		virtual FuncCallParamNode* asFuncCallParam() override { return this; }
		std::string name;

		VISIT_OVERRIDE
	};

	/*
	class FuncCallParamsNode : public Node
	{
	public:
		virtual FuncCallParamsNode* asFuncCallParams() override { return this; }
		std::vector<Node*> children;

		VISIT_OVERRIDE
	};
	*/

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
