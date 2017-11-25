#include <cmath>
#include "parser.h"
#include "ast.h"

#define CHECK_NULL(PTR) if ((PTR) == nullptr) return nullptr;

#define CHECK_OK if (!ok) return nullptr;

namespace halang
{

	Parser::Parser() : 
		Lexer(), ok(true), ast_root(nullptr)
	{ }

	void Parser::StartNode() {
		locations_stack.push(current_tok->location);
	}

	Node* Parser::FinishNode(Node* node) {
		node->begin_location = locations_stack.top();
		locations_stack.pop();
		node->end_location = current_tok->location;
		return node;
	}

	void Parser::Parse()
	{
		this->ast_root = ParseChunk();
	}

	/**********************************/
	/*   Paring                       */
	/**********************************/

	// chunk ::= block
	Node* Parser::parseChunk()
	{
		return parseBlock();
	}


	// block ::= { stat }
	// First(block) = First(stat)
	Node* Parser::ParseBlock()
	{
		StartNode();
		auto _block = make_object<BlockExprNode>();
		
		do
		{
			if (Match(Token::TYPE::SEMICOLON))
				NextToken();
			else
				_block->children.push_back(ParseStatement());

			CHECK_OK

		} while (Match(Token::TYPE::IDENTIFIER) ||
			Match(Token::TYPE::NUMBER) ||
			Match(Token::TYPE::STRING) ||
			Match(Token::TYPE::VAR) ||
			Match(Token::TYPE::OPEN_PAREN) ||
			Match(Token::TYPE::WHILE) ||
			Match(Token::TYPE::BREAK) ||
			Match(Token::TYPE::IF) ||
			Match(Token::TYPE::FUNC) ||
			Match(Token::TYPE::SEMICOLON) ||
			Match(Token::TYPE::RETURN) ||
			Token::IsOperator(*current_tok)
			);
		
		return FinishNode(_block);
	}

	Node* Parser::ParseStatement()
	{
		Node* _node;
		switch (current_tok->type)
		{
		case Token::TYPE::SEMICOLON:
			NextToken();
			return nullptr;
		case Token::TYPE::IDENTIFIER:
			return ParseExpression();
		case Token::TYPE::VAR:
			return ParseVarStmt();
		case Token::TYPE::WHILE:
			return ParseWhileStmt();
		case Token::TYPE::BREAK:
			StartNode();
			NextToken();
			return FinishNode(make_object<BreakStmtNode>());
		case Token::TYPE::CONTINUE:
			StartNode();
			NextToken();
			return FinishNode(make_object<ContinueStmtNode>());
		case Token::TYPE::IF:
			return ParseIfStmt();
		case Token::TYPE::FUNC:
			return ParseFuncDef();
		case Token::TYPE::RETURN:
			return ParseReturnStmt();
		case Token::TYPE::OPEN_BRACKET:
			StartNode();
			NextToken();
			Expect(Token::TYPE::OPEN_BRACKET);
			_node = ParseBlock();

			CHECK_OK

			NextToken();
			Expect(Token::TYPE::CLOSE_BRACKET);
			return FinishNode(_node);
		default:
			return ParseExpression();
		}

	}

	Node* Parser::ParseInvokeExpression(Node *src)
	{
		if (src == nullptr)
		{
			StartNode();
			Expect(Token::TYPE::IDENTIFIER);
			src = FinishNode(
				make_object<IdentifierNode>(NextToken()->GetLiteralValue())
			);
			if (Match(Token::TYPE::OPEN_PAREN)) {
				src = ParseFuncCall(src);

				CHECK_OK
			}
		}
		if (Match(Token::TYPE::OPEN_PAREN)) {
			src = ParseFuncCall(src);

			CHECK_OK
		}
		while (Match(Token::TYPE::DOT))
		{
			StartNode();
			NextToken();
			Expect(Token::TYPE::IDENTIFIER);
			auto second = make_object<IdentifierNode>(
				NextToken()->GetLiteralValue()
			);
			src = make_object <InvokeExprNode>(src, second);
			if (Match(Token::TYPE::OPEN_PAREN))
				src = FinishNode(ParseFuncCall(src));
		}
		return src;
	}

	// varStmt ::= VAR ID (':' TYPE)? '=' exp [ , ID (':' TYPE)? '=' EXP ]
	Node* Parser::ParseVarStmt()
	{
		StartNode();
		Expect(NextToken(), Token::TYPE::VAR);

		VarSubExprNode *_subExpr = nullptr;
		auto _var = make_object<VarStmtNode>();

		do
		{
			_subExpr = ParseVarSubExpr();
			CHECK_OK

			_var->children.push_back(_subExpr);
		} while (Match(Token::TYPE::COMMA));

		return FinishNode(_var);
	}

	VarSubExprNode* Parser::ParseVarSubExpr()
	{
		StartNode();

		auto ptr = make_object<VarSubExprNode>();

		Expect(Token::TYPE::IDENTIFIER);
		auto _id = make_object<IdentifierNode>(
			NextToken()->GetLiteralValue()
		);
		ptr->varName = _id;

		if (Match(Token::TYPE::ASSIGN))
		{
			NextToken();
			ptr->expression = ParseExpression();

			CHECK_OK
		}
		else
			AddError("Expect semicolon or '='.");

		return FinishNode(ptr);
	}

	/// <summary>
	/// Expression ::=
	///		Assignment | BinaryExpr
	/// 
	/// Assignment ::= Variable '=' Expression
	///
	/// BinaryExpr :: BinaryExpr OP BinaryExpr | UnaryExpr
	///
	/// Variable ::= ID ( '[' Expression ']' )?
	///
	/// UnaryExpr ::= ('+' | '-' | '!') UnaryExpr | '(' Expression ')' | FuncCall
	///
	/// FuncCall ::= ID '(' FuncCallArgs ')'
	/// </summary>
	Node* Parser::ParseExpression()
	{
		if (Match(Token::TYPE::IDENTIFIER))
		{
			StartNode();
			Token* _tk = NextToken();
			auto _id = make_object<IdentifierNode>(_tk->GetLiteralValue());
			if (Match(Token::TYPE::DOT) || Match(Token::TYPE::OPEN_PAREN ))
			{
				auto _node = ParseInvokeExpression(_id);
				CHECK_OK
				if (Token::IsOperator(*current_tok))
				{
					auto _op = NextToken();
					return FinishNode(
						ParseBinaryExpr(_node, _op)
					);
				}
				else
					return FinishNode(_node);
			}
			else if (Match(Token::TYPE::ASSIGN))
				return FinishNode(ParseAssignment(_id));
			else if (Token::isOperator(lookahead))
			{
				auto _tk = NextToken();
				return FinishNode(ParseBinaryExpr(_id, _tk));
			}
			else return FinishNode(_id);
		}
		else if (Match(Token::TYPE::OPEN_SQUARE_BRAKET)) {
			return ParseListExpr();
		}
		else
			return ParseBinaryExpr();
	}

	/// <summary>
	/// current version:
	/// Assignment ::=
	///		VAR '=' Expression
	///
	/// future version:
	/// Assignment ::=
	///		VAR { ',' VAR } = Expression { ',' Expression }
	/// </summary>
	Node* Parser::ParseAssignment(IdentifierNode* _id)
	{
		StartNode();
		if (!_id)
		{
			Expect(Token::TYPE::IDENTIFIER);
			std::u16string _str = *lookahead._literal;
			NextToken();
			_id = make_object<IdentifierNode>(_str);
		}

		Expect(NextToken(), Token::TYPE::ASSIGN);
		Node* _exp = parseExpression();

		CHECK_OK

		return FinishNode(
			make_object<AssignmentNode>(_id, _exp)
		);
	}

	ListExprNode* Parser::ParseListExpr()
	{
		StartNode();
		auto list = make_object<ListExprNode>();

		Expect(NextToken(), Token::TYPE::OPEN_SQUARE_BRACKET);
		if (!Match(Token::TYPE::CLOSE_SQUARE_BRACKET))
		{
			Node *node = ParseExpression();
			CHECK_OK

			list->children.push_back(node);

			while (Match(Token::TYPE::COMMA))
			{
				NextToken();
				node = ParseExpression();
				CHECK_OK
				list->children.push_back(node);
			}

			NextToken()
			Expect(Token::TYPE::CLOSE_SQUARE_BRACKET);
		}
		else // ']'
			NextToken();
		
		CHECK_OK

		return FinishNode(list);
	}

	/// <summary>
	/// UnaryExpr ::= 
	///	( '+' | '-' | '!' ) UnaryExpr | Number | VAR | FUNCTIONCALL
	/// </summary>
	Node* Parser::ParseUnaryExpr(OperatorType _op)
	{
		StartNode();

		Node *_node = nullptr,
			*exp = nullptr;
		Token _tk;
		std::u16string _id_name;
		switch (current_tok->type)
		{
		case Token::TYPE::ADD:
			_node = ParseUnaryExpr();
			break;
		case Token::TYPE::SUB:
		case Token::TYPE::NOT:
			_tk = NextToken();
			_node = make_object<UnaryExprNode>(Token::ToOperator(_tk), ParseUnaryExpr());
			break;
		case Token::TYPE::NUMBER:
			_node = make_object<NumberNode>(
				current_tok->GetDoubleValue(), 
				lookahead.maybeInt
			);
			NextToken();
			break;
		case Token::TYPE::STRING:
			_id_name = NextToken()->GetLiteralValue();
			_node = make_object<StringNode>(_id_name);
			break;
		case Token::TYPE::IDENTIFIER:
			_id_name = *NextToken()->GetLiteralValue();
			_node = make_object<IdentifierNode>(_id_name);

			if (Match(Token::TYPE::OPEN_PAREN))
				_node = ParseFuncCall(_node);
			break;
		case Token::TYPE::OPEN_PAREN:
			NextToken();
			exp = ParseExpression();
			Expect(NextToken(), Token::TYPE::CLOSE_PAREN);
			if (Match(Token::TYPE::OPEN_PAREN))
				exp = ParseFuncCall(exp);
			return exp;
		}

		CHECK_OK

		return FinishNode(_node);
	}

	Node* Parser::ParseBinaryExpr(Node* left_exp, Token* left_tk)
	{
		StartNode();

		Node* exp = ParseUnaryExpr();
		CHECK_NULL(exp);
		CHECK_OK
		OperatorType left_op = Token::ToOperator(*left_tk);
		while (true)
		{
			Token* right_tk = current_tok;
			OperatorType right_op = Token::ToOperator(*right_tk);
			if (getPrecedence(left_op) < getPrecedence(right_op))
			{
				NextToken();
				exp = ParseBinaryExpr(exp, right_tk);
				CHECK_OK
			}
			else if (getPrecedence(left_op) == getPrecedence(right_op))
			{
				if (getPrecedence(left_op) == 0)
					return exp;
				CHECK_NULL(left_exp);
				if (left_exp->asNumber() && exp->asNumber())
				{
					auto num1_exp = left_exp->asNumber();
					auto num2_exp = exp->asNumber();
					auto num1 = num1_exp->number;
					auto num2 = num2_exp->number;

					double result = 0;
					auto _maybeInt = num1_exp->maybeInt && num2_exp->maybeInt;

					bool quit = false;
					switch (left_op)
					{
					case OperatorType::ADD:
						result = num1 + num2;
						break;
					case OperatorType::SUB:
						result = num1 - num2;
						break;
					case OperatorType::MUL:
						result = num1 * num2;
						break;
					case OperatorType::DIV:
						result = num1 / num2;
						break;
					case OperatorType::MOD:
						result = static_cast<int>(num1) & static_cast<int>(num2);
						break;
					case OperatorType::POW:
						result = pow(num1, num2);
						break;
					default:
						exp = make_object<BinaryExprNode>(left_op, left_exp, exp);
						quit = true;
					}

					if (!quit)
						exp = make_object<NumberNode>(result, _maybeInt);
				}
				else
					exp = make_object<BinaryExprNode>(left_op, left_exp, exp);
				NextToken();
				return FinishNode(ParseBinaryExpr(exp, right_tk));
			}
			else // left_op > right_op
			{
				if (left_exp)
					exp = make_object<BinaryExprNode>(left_op, left_exp, exp);
				return FinisheNode(exp);
			}
		}
	}

	Node* Parser::ParseIfStmt()
	{
		StartNode();
		Expect(NextToken(), Token::TYPE::IF);
		Expect(NextToken(), Token::TYPE::OPEN_PAREN);
		auto _condition = parseBinaryExpr();
		Expect(NextToken(), Token::TYPE::CLOSE_PAREN);

		auto _stmt = ParseStatement();
		CHECK_NULL(_stmt)

		Node* _else = nullptr;
		if (Match(Token::TYPE::ELSE))
			_else = ParseElseStmt();
		return FinishNode(
			make_object<IfStmtNode>(_condition, _stmt, _else)
		);
	}

	Node* Parser::ParseElseStmt()
	{
		StartNode();

		Expect(NextToken(), Token::TYPE::ELSE);
		if (Match(Token::TYPE::IF))
			return ParseIfStmt();

		auto _stmt = ParseStatement();
		CHECK_OK
		return FinishNode(_stmt);
	}

	Node* Parser::ParseWhileStmt()
	{
		StartNode();

		Expect(NextToken(), Token::TYPE::WHILE);
		Expect(NextToken(), Token::TYPE::OPEN_PAREN); // (
		auto _condition = ParseBinaryExpr();
		Expect(NextToken(), Token::TYPE::CLOSE_PAREN);
		auto _stmt = ParseStatement();
		CHECK_OK
		return FinishNode(
			make_object<WhileStmtNode>(_condition, _stmt)
		);
	}

	ClassDefNode* Parser::ParseClassDef()
	{
		StartNode();
		Expect(NextToken(), Token::TYPE::CLASS);
		auto cl = make_object<ClassDefNode>();

		Expect(lookahead, Token::TYPE::IDENTIFIER);
		cl->name = make_object<IdentifierNode>(NextToken()->GetLiteralValue());
		if (Match(Token::TYPE::SEMICOLON))
		{
			NextToken();
			cl->extendName = make_object<IdentifierNode>(NextToken()->GetLiteralValue());
		}
		Expect(NextToken(), Token::TYPE::OPEN_BRACKET);
		// { ------------------------------------------
		while (!Match(Token::TYPE::CLOSE_BRACKET))
		{
			cl->members.push_back(ParseClassMember());
			CHECK_OK
		}
		// } ------------------------------------------
		Expect(NextToken(), Token::TYPE::CLOSE_BRAKET);
		return reinterpret_cast<ClassDefNode*> (
			FinishNode(cl)
		);
	}

	Node* Parser::ParseClassMember()
	{
		StartNode();
		Node* result = nullptr;
		Expect(lookahead, Token::TYPE::IDENTIFIER);
		auto id = make_object<IdentifierNode>(*NextToken()._literal);

		if (Match(Token::TYPE::OPEN_PAREN))
		{
			auto _func = make_object<FuncDefNode>();
			_func->name = id;
			NextToken();
			while (!Match(Token::TYPE::CLOSE_PAREN))
			{
				_func->parameters.push_back(ParseFuncDefParam());
				CHECK_OK
				if (Match(Token::TYPE::COMMA))
				{
					NextToken();
					continue;
				}

				Expect(Token::TYPE::OPEN_BRACKET);
				NextToken();
				_func->block = reinterpret_cast<BlockExprNode*>(ParseBlock());
				CHECK_OK
				Expect( Token::TYPE::CLOSE_BRACKET);
				NextToken();
			}
			Expect(NextToken(), Token::TYPE::CLOSE_PAREN);

		}
		else if (Match(Token::TYPE::ASSIGN))
		{
			auto _expr = make_object<AssignmentNode>();
			_expr->identifier = id;
			_expr->expression = ParseExpression();
			CHECK_OK
		}
		else
			AddError("<Class Member Definition>UnExpected Token");

		return FinishNode(result);
	}

	Node* Parser::ParseFuncDef()
	{
		StartNode();
		// def new function
		auto _func = make_object<FuncDefNode>();
		Expect(NextToken(), Token::TYPE::FUNC);

		if (Match(Token::TYPE::IDENTIFIER))
			_func->name = make_object<IdentifierNode>(*NextToken()._literal);
		else
			_func->name = nullptr;

		NextToken();
		Expect(Token::TYPE::OPEN_PAREN);
		while (Match(Token::TYPE::IDENTIFIER))
		{
			auto param = ParseFuncDefParam();
			CHECK_OK
			_func->parameters.push_back(param);
			if (Match(Token::TYPE::COMMA))
				NextToken();
			else if (Match(Token::TYPE::CLOSE_PAREN))
				break;
			else
				AddError("UnExpected def params.");
		}
		Expect(NextToken(), Token::TYPE::CLOSE_PAREN);

		Expect(NextToken(), Token::TYPE::OPEN_BRACKET);
		_func->block = reinterpret_cast<BlockExprNode*>(ParseBlock());
		CHECK_OK
		Expect(NextToken(), Token::TYPE::CLOSE_BRACKET);
		return FinishNode(_func);
	}

	FuncDefParamNode* Parser::ParseFuncDefParam()
	{
		StartNode();
		auto param = make_object<FuncDefParamNode>();
		Expect(Token::TYPE::IDENTIFIER);
		param->name = NextToken()->GetLiteralValue();
		return FinishNode(param);
	}

	Node* Parser::ParseFuncCall(Node* exp)
	{
		StartNode();
		FuncCallNode* _node = make_object<FuncCallNode>(exp);
		// FuncCallParamNode* _params = nullptr;

		Expect(, Token::TYPE::OPEN_PAREN);
		NextToken();
		while (!Match(Token::TYPE::CLOSE_PAREN))
		{
			auto node = ParseExpression();
			CHECK_OK

			_node->parameters.push_back(node);
			if (Match(Token::TYPE::COMMA))
				NextToken();
			else if (Match(Token::TYPE::CLOSE_PAREN))
				break;
			else
				AddError("Expect identifier or comma.");
		}
		Expect(NextToken(), Token::TYPE::CLOSE_PAREN);

		while (Match(Token::TYPE::OPEN_PAREN))
		{
			NextToken();
			_node = make_object<FuncCallNode>(_node);

			while (!Expect(Token::TYPE::CLOSE_PAREN))
			{
				auto node = ParseExpression();
				CHECK_OK

				_node->parameters.push_back(node);
				if (Match(Token::TYPE::COMMA))
					continue;
				else if (Match(Token::TYPE::CLOSE_PAREN))
					break;
				else
					ReportError("Expect identifier or comma.");
			}

			Expect(NextToken(), Token::TYPE::CLOSE_PAREN);
		}
		return FinishNode(_node);
	}

	Node* Parser::ParseReturnStmt()
	{
		StartNode();
		Expect(Token::TYPE::RETURN);
		auto exp = ParseExpression();
		CHECK_OK
		return FinishNode(
			make_object<ReturnStmtNode>(exp)
		);
	}

	Parser::~Parser()
	{
	}

}
