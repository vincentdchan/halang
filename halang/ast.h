#pragma once
#include <cinttypes>
#include <vector>
#include <memory>
#include "token.h"
#include "string.h"

namespace halang
{

#define NODE_LIST(V) \
	V(Assignment) \
	V(Number) \
	V(Identifier) \
	V(String) \
	V(InvokeExpr) \
	V(ListExpr) \
	V(BinaryExpr) \
	V(UnaryExpr) \
	V(BlockExpr) \
	V(VarStmt) \
	V(VarSubExpr) \
	V(IfStmt) \
	V(WhileStmt) \
	V(BreakStmt) \
	V(ReturnStmt) \
	V(PrintStmt) \
	V(FuncDef) \
	V(FuncDefParam)\
	V(FuncCall) \

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
		virtual StringNode* asString() { return nullptr; }
		virtual IdentifierNode* asIdentifier() { return nullptr; }
		virtual NumberNode* asNumber() { return nullptr; }
		virtual AssignmentNode* asAssignment() { return nullptr; }
		virtual InvokeExprNode* asInvokeExpr() { return nullptr; }
		virtual ListExprNode* asListExpr() { return nullptr; }
		virtual UnaryExprNode* asUnaryExpression() { return nullptr; }
		virtual BlockExprNode* asBlockExpression() { return nullptr; }
		virtual BinaryExprNode* asBinaryExpression() { return nullptr; }
		virtual VarStmtNode* asVarStmt() { return nullptr; }
		virtual VarSubExprNode* asVarSubExpr() { return nullptr; }
		virtual IfStmtNode* asIfStmt() { return nullptr; }
		virtual WhileStmtNode* asWhileStmt() { return nullptr; }
		virtual BreakStmtNode* asBreakStmt() { return nullptr; }
		virtual ReturnStmtNode* asReturnStmt() { return nullptr; }
		virtual FuncDefNode* asFuncDef() { return nullptr; }
		virtual FuncDefParamNode* asFuncDefParam() { return nullptr; }
		virtual FuncCallNode* asFuncCall() { return nullptr; }
		virtual PrintStmtNode* asPrintStmt() { return nullptr; }

		virtual void visit(Visitor*) = 0;
	};

	/// <summary>
	/// Represent the characters in the quotes
	/// </summary>
	class StringNode : public Node
	{
	public:
		StringNode(IString _content) : content(_content)
		{}

		virtual StringNode* asString() override { return this; }
		IString content;

		VISIT_OVERRIDE
	};

	/// <summary>
	/// Represent a number, integer or double.
	/// </summary>
	class NumberNode : public Node
	{
	public:
		NumberNode(double _num = 0, bool _mi = false) :
			number(_num), maybeInt(_mi)
		{ }
		virtual NumberNode* asNumber() override { return this; }
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
		IdentifierNode(const std::string& _str) : name(_str)
		{}
		virtual IdentifierNode* asIdentifier() override { return this; }

		std::string name;

		VISIT_OVERRIDE
	};

	/// <summary>
	/// InvokeExpression ::= InvokeExpression ID | ID
	/// </summary>
	class InvokeExprNode : public Node
	{
	public:
		InvokeExprNode(Node* src = nullptr, IdentifierNode* _id = nullptr):
			source(src), id(_id)
		{}

		virtual InvokeExprNode* asInvokeExpr() { return this; }

		Node* source;
		IdentifierNode* id;

		VISIT_OVERRIDE
	};

	/// <summary>
	/// Assignment ::= Identifier '=' Expression
	/// </summary>
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

	/// <summary>
	/// ListExpression ::= '[' ListExpressionChildren ']'
	/// ListExpressionChildren ::= NULL | Expression ListExpressionChildren
	/// </summary>
	class ListExprNode : public Node
	{
	public:
		ListExprNode() {}
		std::vector<Node*> children;

		virtual ListExprNode* asListExpr() override { return this; }

		VISIT_OVERRIDE
	};

	/// <summary>
	/// Block ::= '{' {Expression} '}'
	/// </summary>
	class BlockExprNode : public Node
	{
	public:
		BlockExprNode() {}
		std::vector<Node*> children;
		virtual BlockExprNode* asBlockExpression() override { return this; }

		VISIT_OVERRIDE
	};

	/// <summary>
	/// UnaryExpression ::=  '+' | '-' | '!' ,  Identifier | Number | '(' Expression ')'
	/// </summary>
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

	/// <summary>
	/// BinaryExpression ::= BinaryExpression' OP BinaryExpression' 
	/// BinaryExpression' ::= UnaryExpression | Number | Identifier | '(' BinaryExpression ')'
	/// </summary>
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


	/// <summary>
	/// VarStatement ::= "var" VarSubExpression {',' VarSubExpression} 
	/// VarSubExpression ::= Identifier VarInitExpression
	/// VarInitExpression ::= ':' Identifier |  '=' Expression | ':' Identifier '=' Expression
	/// </summary>
	class VarStmtNode : public Node
	{
	public:
		virtual VarStmtNode* asVarStmt() override { return this; }
		std::vector<VarSubExprNode*> children;

		VISIT_OVERRIDE
	};

	/// <summary>
	/// VarSubExpression ::= Identifier VarInitExpression
	/// VarInitExpression ::= ':' Identifier |  '=' Expression | ':' Identifier '=' Expression
	/// </summary>
	class VarSubExprNode : public Node
	{
	public:
		VarSubExprNode() :
			varName(nullptr), expression(nullptr)
		{}
		virtual VarSubExprNode* asVarSubExpr() override { return this; }
		IdentifierNode* varName;
		Node* expression;

		VISIT_OVERRIDE
	};

	/// <summary>
	/// IfStatement ::= 'if' '(' Expression ')' TrueBranch { FalseBranch }
	/// TrueBranch ::= Branch
	/// FalseBranch ::= 'else' ( IfStatement | Branch ) 
	/// Branch ::= Expression | Block
	/// </summary>
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

	/// <summary>
	/// WhileStatement ::= 'while' '(' Expression ')' (Expression | Block)
	/// </summary>
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

	/// <summary>
	/// Break ::= 'break'
	/// </summary>
	class BreakStmtNode : public Node
	{
	public:
		virtual BreakStmtNode* asBreakStmt() override { return this; }

		VISIT_OVERRIDE
	};

	/// <summary>
	/// Return ::= 'return'
	/// </summary>
	class ReturnStmtNode : public Node
	{
	public:
		ReturnStmtNode(Node * exp = nullptr) : expression(exp)
		{}
		virtual ReturnStmtNode* asReturnStmt() override { return this; }
		Node* expression;

		VISIT_OVERRIDE
	};

	/// <summary>
	/// FuncDefinition ::= 'func' '(' ParamList ')' '{' { Expression } '}
	/// ParamList ::=  Identifier ':' Identifier { ',' Identifier ':' Identifier }
	/// </summary>
	class FuncDefNode : public Node
	{
	public:
		FuncDefNode() {}
		virtual FuncDefNode* asFuncDef() override { return this; }
		IdentifierNode* name;
		std::vector<FuncDefParamNode*> parameters;

		BlockExprNode* block;

		VISIT_OVERRIDE
	};

	/// <summary>
	/// ParamList ::=  Identifier ':' Identifier { ',' Identifier ':' Identifier }
	/// </summary>
	class FuncDefParamNode : public Node
	{
	public:
		FuncDefParamNode()
		{}
		virtual FuncDefParamNode* asFuncDefParam() override { return this; }
		std::string name;

		VISIT_OVERRIDE
	};

	/// <summary>
	/// FunctionCall ::= Expression  '(' FunctionCallParams ')'
	/// FunctionCallParams ::= Identifier { ',' Identifier }
	/// </summary>
	class FuncCallNode : public Node
	{
	public:
		FuncCallNode(Node* _exp = nullptr) :
			exp(_exp)
		{}
		virtual FuncCallNode* asFuncCall() override { return this; }
		Node* exp; // maybe identifier, maybe another func  foo(a) foo(a)(b)(b)
		std::vector<Node*> parameters;

		VISIT_OVERRIDE
	};

	/// <summary>
	/// PrintStatement ::= 'print' Expression
	/// </summary>
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
