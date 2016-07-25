#pragma once

#include <cinttypes>
#include <stack>
#include <list>
#include <iostream>
#include "lex.h"
#include "ast.h"
#include "util.h"

namespace parser
{
	using namespace lex;

	class Parser final : public utils::_MessageContainer
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

		Node* parseChunk();
		Node* parseBlock();
		Node* parseStatement();
		Node* parseLetExpr();
		Node* parseAssignment();
		Node* parseUnaryExpr();
		Node* parseExpression();
		Node* parseBinaryExpr();
		Node* parseIfStmt();
		Node* parseElseStmt();
		Node* parseWhileStmt();
		Node* parseFunctionStmt();
		Node* parseFunctionParameters();
		Parser() = delete;
		Parser(const Parser&) = delete;
		Parser& operator=(const Parser&) = delete;
		Lexer& lexer;
		std::list<Node*> _node_list;
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
				ReportError(string("Unexpected token: ") + TokenName[num]);
				ok = false;
				return false;
			}
		}

		//================================================
		// record all the pointers of Node value.
		// it can be clear when the parser is destructing
		//
		// make_node is similar to make_unqiue
		template<typename _Ty, typename... _Types>
		_Ty* make_node(_Types&&... args)
		{
			_Ty* _node = new _Ty(std::forward<_Types>(args)...);
			_node_list.push_back(_node);
			make_unique<int>(3);
			return _node;
		}

	};

}
