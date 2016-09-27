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
		public utils::_MessageContainer<std::string>, private utils::PointerContainer<Node>
	{
	public:
		enum struct MESSAGE_TYPE
		{
			NORMAL,
			WARNING,
			ERROR
		};
		Parser(Lexer& _lex);
		Parser(Parser&& _parser);
		Token nextToken();
		Token lookahead;
		void parse();
		bool isOk() const { return ok; }
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
		Node*				parseVarStmt();
		VarSubExprNode*		parseVarSubExpr();
		ListExprNode*		parseListExpr();
		Node*				parseUnaryExpr(OperatorType _op = OperatorType::ILLEGAL_OP);
		Node*				parseBinaryExpr(Node* left_exp = nullptr, Token left_tk = Token());
		Node*				parseIfStmt();
		Node*				parseElseStmt();
		Node*				parseWhileStmt();
		Node*				parseFuncDef();
		Node*				parseFuncDefParam();
		Node*				parseFuncCall(Node* _exp = nullptr);
		Node*				parseReturnStmt();
		Node*				parsePrintStmt();
		Parser() = delete;
		Parser(const Parser&) = delete;
		Parser& operator=(const Parser&) = delete;
		Lexer& lexer;

		inline void add_error(Token t, char* msg)
		{
			stringstream ss;
			ss << "Line: " << t.location.line << ": " << msg;
			ReportError(ss.str());
		}

		inline void add_error(Token t, const string& msg)
		{
			stringstream ss;
			ss << "Line: " << t.location.line << ": " << msg;
			ReportError(ss.str());
		}

		inline bool match(Token::TYPE t) { return lookahead.type == t; } // just judge
		inline bool expect(Token::TYPE t)
		{
			return expect(lookahead, t);
		}

		inline bool expect(Token _c, Token::TYPE t)
		{
			if (_c.type == t)
				return true;
			else
			{
				auto num = static_cast<int>(_c.type);
				// warning: here may cause crash if num is not in range
				this->add_error(_c, string("Unexpected token: ") + TokenName[num]);
				ok = false;
				return false;
			}
		}

	};

};
