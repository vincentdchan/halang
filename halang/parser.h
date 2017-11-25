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

	class Parser : 
		public Lexer, private utils::PointerContainer<Node>
	{
	public:
		enum struct MESSAGE_TYPE
		{
			NORMAL,
			WARNING,
			ERROR
		};
		Parser();

		void ParseProgram();

		bool IsOK() const { return ok; }
		inline Node* getRoot() { return ast_root; }

		~Parser();
	protected:
		void StartNode();
		Node* FinishNode(Node* node);

	private:
		bool ok;

		ProgramNode* ast_root;

		std::stack<Location> locations_stack;

		Node*				ParseIdentifier();
		Node*				ParseStatement();
		Node*				ParseLetStatement();
		Node*				ParseIfStatement();
		Node*				ParseWhileStatement();
		Node*				ParseReturnStatement();
		Node*				ParseDefStatement();
		Node*				ParseExpression();
		Node*				ParseCallExpression(Node* src = nullptr);
		Node*				ParseMemberExpression(Node* src = nullptr);
		Node*				ParseAssignExpression(IdentifierNode* = nullptr);
		Node*				ParseListExpression();
		Node*				ParseMaybeUnary();
		Node*				ParseExpressionUnit();
		Node*				ParseMaybeCallExpression();

		Node*				ParseBinaryExpr(Node* left_exp = nullptr, Token* left_tk = nullptr);

		Parser(const Parser&) = delete;
		Parser& operator=(const Parser&) = delete;

		inline void AddError(const string& msg)
		{
			ok = false;
			stringstream ss;
			Location loc = locations_stack.top();
			ss << "Line: " << loc.line 
				<< ":" << loc.column << ": " << msg;
			ReportError(ss.str());
		}

		inline bool Expect(Token::TYPE t)
		{

			if (current_tok->type == t)
				return true;
			else
			{
				auto num = static_cast<int>(current_tok->type);
				auto expected_num = static_cast<int>(t);
				// warning: here may cause crash if num is not in range
				AddError(
					string("Unexpected token: ") + TokenName[num] +
					string(" Expected: ") + TokenName[expected_num]
				);
				return false;
			}
		}

	};

};
