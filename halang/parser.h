#pragma once

#include <cinttypes>
#include <stack>
#include <list>
#include <iostream>
#include <sstream>
#include "lex.h"
#include "ast.h"
#include "util.h"

namespace halang
{

	class Parser final : 
		public utils::MessageContainer, private utils::PointerContainer<Node>
	{
	public:
		enum struct MESSAGE_TYPE
		{
			NORMAL,
			WARNING,
			ERROR
		};
		Parser();

		void Parse();
		bool IsOK() const { return ok; }
		inline Node* getRoot() { return ast_root; }

		~Parser();
	private:
		bool ok;
		Node* ast_root;

		Node*				parseChunk();
		Node*				parseBlock();
		Node*				parseStatement();
		Node*				parseAssignment(IdentifierNode* = nullptr);
		Node*				parseExpression();
		Node*				parseInvokeExpression(Node* src = nullptr);
		Node*				parseVarStmt();
		VarSubExprNode*		parseVarSubExpr();
		ListExprNode*		parseListExpr();
		Node*				parseUnaryExpr(OperatorType _op = OperatorType::ILLEGAL_OP);
		Node*				parseBinaryExpr(Node* left_exp = nullptr, Token left_tk = Token());
		Node*				parseIfStmt();
		Node*				parseElseStmt();
		Node*				parseWhileStmt();
		ClassDefNode*		parseClassDef();
		Node*				parseClassMember();
		Node*				parseFuncDef();
		FuncDefParamNode*	parseFuncDefParam();
		Node*				parseFuncCall(Node* _exp = nullptr);
		Node*				parseReturnStmt();
		Parser(const Parser&) = delete;
		Parser& operator=(const Parser&) = delete;

		inline void AddError(Token t, char* msg)
		{
			stringstream ss;
			ss << "Line: " << t.location.line << ": " << msg;
			ReportError(ss.str());
		}

		inline void AddError(Token t, const string& msg)
		{
			stringstream ss;
			ss << "Line: " << t.location.line << ": " << msg;
			ReportError(ss.str());
		}

		inline bool Expect(Token::TYPE t)
		{
			return Expect(lookahead, t);
		}

		inline bool Expect(Token _c, Token::TYPE t)
		{
			if (_c.type == t)
				return true;
			else
			{
				auto num = static_cast<int>(_c.type);
				// warning: here may cause crash if num is not in range
				this->AddError(_c, string("UnExpected token: ") + TokenName[num]);
				ok = false;
				return false;
			}
		}

	};

};
