#include <cmath>
#include "parser.h"
#include "ast.h"

#define CHECK_NULL(PTR) if ((PTR) == nullptr) return nullptr;

namespace halang
{

	Parser::Parser() : 
		lexer(_lex), ok(true), ast_root(nullptr)
	{ }

	void Parser::Parse()
	{
		this->ast_root = this->parseChunk();
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
	Node* Parser::parseBlock()
	{
		auto _block = make_object<BlockExprNode>();
		
		do
		{
			if (Match(Token::TYPE::SEMICOLON))
				NextToken();
			else
				_block->children.push_back(parseStatement());
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
			Token::isOperator(lookahead)
			);

		return _block;
	}

	Node* Parser::parseStatement()
	{
		Node* _node;
		switch (lookahead.type)
		{
		case Token::TYPE::SEMICOLON:
			NextToken();
			return nullptr;
		case Token::TYPE::IDENTIFIER:
			return parseExpression();
		case Token::TYPE::VAR:
			return parseVarStmt();
		case Token::TYPE::WHILE:
			return parseWhileStmt();
		case Token::TYPE::BREAK:
			NextToken();
			return make_object<BreakStmtNode>();
		case Token::TYPE::CONTINUE:
			NextToken();
			return make_object<ContinueStmtNode>();
		case Token::TYPE::IF:
			return parseIfStmt();
		case Token::TYPE::FUNC:
			return parseFuncDef();
		case Token::TYPE::RETURN:
			return parseReturnStmt();
		case Token::TYPE::OPEN_BRAKET:
			Expect(NextToken(), Token::TYPE::OPEN_BRAKET);
			_node = parseBlock();
			Expect(NextToken(), Token::TYPE::CLOSE_BRAKET);
			return _node;
		default:
			return parseExpression();
		}

	}

	Node* Parser::parseInvokeExpression(Node *src)
	{
		if (src == nullptr)
		{
			Expect(lookahead, Token::TYPE::IDENTIFIER);
			src = make_object<IdentifierNode>(*NextToken()._literal);
			if (Match(Token::TYPE::OPEN_PAREN))
				src = parseFuncCall(src);
		}
		if (Match(Token::TYPE::OPEN_PAREN))
			src = parseFuncCall(src);
		while (Match(Token::TYPE::DOT))
		{
			NextToken();
			Expect(lookahead, Token::TYPE::IDENTIFIER);
			auto second = make_object<IdentifierNode>(*NextToken()._literal);
			src = make_object <InvokeExprNode>(src, second);
			if (Match(Token::TYPE::OPEN_PAREN))
				src = parseFuncCall(src);
		}
		return src;
	}

	// varStmt ::= VAR ID (':' TYPE)? '=' exp [ , ID (':' TYPE)? '=' EXP ]
	Node* Parser::parseVarStmt()
	{
		Expect(NextToken(), Token::TYPE::VAR);

		VarSubExprNode *_subExpr = nullptr;
		auto _var = make_object<VarStmtNode>();

		do
		{
			_subExpr = parseVarSubExpr();
			_var->children.push_back(_subExpr);
		} while (Match(Token::TYPE::COMMA));

		return _var;
	}

	VarSubExprNode* Parser::parseVarSubExpr()
	{
		auto ptr = make_object<VarSubExprNode>();

		Expect(lookahead, Token::TYPE::IDENTIFIER);
		auto _id = make_object<IdentifierNode>(*NextToken()._literal);
		ptr->varName = _id;

		if (Match(Token::TYPE::ASSIGN))
		{
			NextToken();
			ptr->expression = parseExpression();
		}
		else
			ReportError("Expect semicolon or '='.");

		return ptr;
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
	Node* Parser::parseExpression()
	{
		if (Match(Token::TYPE::IDENTIFIER))
		{
			Token _tk = NextToken();
			auto _id = make_object<IdentifierNode>(*_tk._literal);
			if (Match(Token::TYPE::DOT) || Match(Token::TYPE::OPEN_PAREN ))
			{
				auto _node = parseInvokeExpression(_id);
				if (Token::isOperator(lookahead))
				{
					auto _op = NextToken();
					return parseBinaryExpr(_node, _op);
				}
				else
					return _node;
			}
			else if (Match(Token::TYPE::ASSIGN))
				return parseAssignment(_id);
			else if (Token::isOperator(lookahead))
			{
				auto _tk = NextToken();
				return parseBinaryExpr(_id, _tk);
			}
			else return _id;
		}
		else if (Match(Token::TYPE::OPEN_SQUARE_BRAKET)) {
			return parseListExpr();
		}
		else
			return parseBinaryExpr();
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
	Node* Parser::parseAssignment(IdentifierNode* _id)
	{
		if (!_id)
		{
			Expect(Token::TYPE::IDENTIFIER);
			std::u16string _str = *lookahead._literal;
			NextToken();
			_id = make_object<IdentifierNode>(_str);
		}

		Expect(NextToken(), Token::TYPE::ASSIGN);
		Node* _exp = parseExpression();
		return make_object<AssignmentNode>(_id, _exp);
	}

	ListExprNode* Parser::parseListExpr()
	{
		auto list = make_object<ListExprNode>();

		Expect(NextToken(), Token::TYPE::OPEN_SQUARE_BRAKET);
		if (!Match(Token::TYPE::CLOSE_SQUARE_BRAKET))
		{
			Node *node = parseExpression();
			list->children.push_back(node);

			while (Match(Token::TYPE::COMMA))
			{
				NextToken();
				node = parseExpression();
				list->children.push_back(node);
			}

			Expect(NextToken(), Token::TYPE::CLOSE_SQUARE_BRAKET);
		}
		else // ']'
			NextToken();

		return list;
	}

	/// <summary>
	/// UnaryExpr ::= 
	///	( '+' | '-' | '!' ) UnaryExpr | Number | VAR | FUNCTIONCALL
	/// </summary>
	Node* Parser::parseUnaryExpr(OperatorType _op)
	{
		Node *_node = nullptr,
			*exp = nullptr;
		Token _tk;
		std::u16string _id_name;
		switch (lookahead.type)
		{
		case Token::TYPE::ADD:
			_node = parseUnaryExpr();
			break;
		case Token::TYPE::SUB:
		case Token::TYPE::NOT:
			_tk = NextToken();
			_node = make_object<UnaryExprNode>(Token::toOperator(_tk), parseUnaryExpr());
			break;
		case Token::TYPE::NUMBER:
			_node = make_object<NumberNode>(lookahead._double, lookahead.maybeInt);
			NextToken();
			break;
		case Token::TYPE::STRING:
			_id_name = *NextToken()._literal;
			_node = make_object<StringNode>(_id_name);
			break;
		case Token::TYPE::IDENTIFIER:
			_id_name = *NextToken()._literal;
			_node = make_object<IdentifierNode>(_id_name);

			if (Match(Token::TYPE::OPEN_PAREN))
				_node = parseFuncCall(_node);
			break;
		case Token::TYPE::OPEN_PAREN:
			NextToken();
			exp = parseExpression();
			Expect(NextToken(), Token::TYPE::CLOSE_PAREN);
			if (Match(Token::TYPE::OPEN_PAREN))
				exp = parseFuncCall(exp);
			return exp;
		}

		return _node;
	}

	Node* Parser::parseBinaryExpr(Node* left_exp, Token left_tk)
	{
		Node* exp = parseUnaryExpr();
		CHECK_NULL(exp);
		OperatorType left_op = Token::toOperator(left_tk);
		while (true)
		{
			Token right_tk = lookahead;
			OperatorType right_op = Token::toOperator(right_tk);
			if (getPrecedence(left_op) < getPrecedence(right_op))
			{
				NextToken();
				exp = parseBinaryExpr(exp, right_tk);
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
				return parseBinaryExpr(exp, right_tk);
			}
			else // left_op > right_op
			{
				if (left_exp)
					exp = make_object<BinaryExprNode>(left_op, left_exp, exp);
				return exp;
			}
		}
	}

	Node* Parser::parseIfStmt()
	{
		Expect(NextToken(), Token::TYPE::IF);
		Expect(NextToken(), Token::TYPE::OPEN_PAREN);
		auto _condition = parseBinaryExpr();
		Expect(NextToken(), Token::TYPE::CLOSE_PAREN);

		auto _stmt = parseStatement();
		CHECK_NULL(_stmt)

		Node* _else = nullptr;
		if (Match(Token::TYPE::ELSE))
			_else = parseElseStmt();
		return make_object<IfStmtNode>(_condition, _stmt, _else);
	}

	Node* Parser::parseElseStmt()
	{
		Expect(NextToken(), Token::TYPE::ELSE);
		if (Match(Token::TYPE::IF))
			return parseIfStmt();

		auto _stmt = parseStatement();
		return _stmt;
	}

	Node* Parser::parseWhileStmt()
	{
		Expect(NextToken(), Token::TYPE::WHILE);
		Expect(NextToken(), Token::TYPE::OPEN_PAREN); // (
		auto _condition = parseBinaryExpr();
		Expect(NextToken(), Token::TYPE::CLOSE_PAREN);
		auto _stmt = parseStatement();
		return make_object<WhileStmtNode>(_condition, _stmt);
	}

	ClassDefNode* Parser::parseClassDef()
	{
		Expect(NextToken(), Token::TYPE::CLASS);
		auto cl = make_object<ClassDefNode>();

		Expect(lookahead, Token::TYPE::IDENTIFIER);
		cl->name = make_object<IdentifierNode>(*NextToken()._literal);
		if (Match(Token::TYPE::SEMICOLON))
		{
			NextToken();
			cl->extendName = make_object<IdentifierNode>(*NextToken()._literal);
		}
		Expect(NextToken(), Token::TYPE::OPEN_BRAKET);
		// { ------------------------------------------
		while (!Match(Token::TYPE::CLOSE_BRAKET))
		{
			cl->members.push_back(parseClassMember());
		}
		// } ------------------------------------------
		Expect(NextToken(), Token::TYPE::CLOSE_BRAKET);
		return cl;
	}

	Node* Parser::parseClassMember()
	{
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
				_func->parameters.push_back(parseFuncDefParam());
				if (Match(Token::TYPE::COMMA))
				{
					NextToken();
					continue;
				}

				Expect(NextToken(), Token::TYPE::OPEN_BRAKET);
				_func->block = reinterpret_cast<BlockExprNode*>(parseBlock());
				Expect(NextToken(), Token::TYPE::CLOSE_BRAKET);
			}
			Expect(NextToken(), Token::TYPE::CLOSE_PAREN);

		}
		else if (Match(Token::TYPE::ASSIGN))
		{
			auto _expr = make_object<AssignmentNode>();
			_expr->identifier = id;
			_expr->expression = parseExpression();
		}
		else
			this->AddError(lookahead, "<Class Member Definition>UnExpected Token");

		return result;
	}

	Node* Parser::parseFuncDef()
	{
		// def new function
		auto _func = make_object<FuncDefNode>();
		Expect(NextToken(), Token::TYPE::FUNC);

		if (Match(Token::TYPE::IDENTIFIER))
			_func->name = make_object<IdentifierNode>(*NextToken()._literal);
		else
			_func->name = nullptr;

		Expect(NextToken(), Token::TYPE::OPEN_PAREN);
		while (Match(Token::TYPE::IDENTIFIER))
		{
			auto param = parseFuncDefParam();
			_func->parameters.push_back(param);
			if (Match(Token::TYPE::COMMA))
				NextToken();
			else if (Match(Token::TYPE::CLOSE_PAREN))
				break;
			else
				ReportError("UnExpected def params.");
		}
		Expect(NextToken(), Token::TYPE::CLOSE_PAREN);

		Expect(NextToken(), Token::TYPE::OPEN_BRAKET);
		_func->block = reinterpret_cast<BlockExprNode*>(parseBlock());
		Expect(NextToken(), Token::TYPE::CLOSE_BRAKET);
		return _func;
	}

	FuncDefParamNode* Parser::parseFuncDefParam()
	{
		auto param = make_object<FuncDefParamNode>();
		Expect(lookahead, Token::TYPE::IDENTIFIER);
		param->name = *NextToken()._literal;
		return param;
	}

	Node* Parser::parseFuncCall(Node* exp)
	{
		FuncCallNode* _node = make_object<FuncCallNode>(exp);
		// FuncCallParamNode* _params = nullptr;

		Expect(NextToken(), Token::TYPE::OPEN_PAREN);
		while (!Match(Token::TYPE::CLOSE_PAREN))
		{
			auto node = parseExpression();

			_node->parameters.push_back(node);
			if (Match(Token::TYPE::COMMA))
				NextToken();
			else if (Match(Token::TYPE::CLOSE_PAREN))
				break;
			else
				ReportError("Expect identifier or comma.");
		}
		Expect(NextToken(), Token::TYPE::CLOSE_PAREN);

		while (Match(Token::TYPE::OPEN_PAREN))
		{
			NextToken();
			_node = make_object<FuncCallNode>(_node);

			while (!Expect(lookahead, Token::TYPE::CLOSE_PAREN))
			{
				auto node = parseExpression();

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
		return _node;
	}

	Node* Parser::parseReturnStmt()
	{
		Expect(NextToken(), Token::TYPE::RETURN);
		auto exp = parseExpression();
		return make_object<ReturnStmtNode>(exp);
	}

	Parser::~Parser()
	{
	}

}
