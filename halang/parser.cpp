
#include "stdafx.h"
#include "parser.h"
#include "ast.h"

#define CHECK_NULL(PTR) if ((PTR) == nullptr) return nullptr;

namespace halang
{

	Parser::Parser(Lexer& _lex) : 
		lexer(_lex), ok(true), ast_root(nullptr)
	{
		nextToken();
	}

	Parser::Parser(Parser&& _parser) :
		_MessageContainer(_parser),
		lexer(_parser.lexer), ok(_parser.ok), 
		ast_root(move(_parser.ast_root))
	{ }

	void Parser::parse()
	{
		this->ast_root = this->parseChunk();
	}

	Token Parser::nextToken()
	{
		Token tmp = move(lookahead);
		lookahead = lexer.read();
		return tmp;
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
		auto _block = make_node<BlockExprNode>();
		
		do
		{
			if (match(Token::TYPE::SEMICOLON))
				nextToken();
			else
				_block->children.push_back(parseStatement());
		} while (match(Token::TYPE::IDENTIFIER) ||
			match(Token::TYPE::NUMBER) ||
			match(Token::TYPE::STRING) ||
			match(Token::TYPE::VAR) ||
			match(Token::TYPE::OPEN_PAREN) ||
			match(Token::TYPE::WHILE) ||
			match(Token::TYPE::IF) ||
			match(Token::TYPE::FUNC) ||
			match(Token::TYPE::SEMICOLON) ||
			match(Token::TYPE::PRINT) ||
			match(Token::TYPE::RETURN) ||
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
			nextToken();
			return nullptr;
		case Token::TYPE::VAR:
			return parseVarStmt();
		case Token::TYPE::WHILE:
			return parseWhileStmt();
		case Token::TYPE::BREAK:
			nextToken();
			return make_node<BreakStmtNode>();
		case Token::TYPE::IF:
			return parseIfStmt();
		case Token::TYPE::FUNC:
			return parseFuncDef();
		case Token::TYPE::RETURN:
			return parseReturnStmt();
		case Token::TYPE::OPEN_BRAKET:
			expect(nextToken(), Token::TYPE::OPEN_BRAKET);
			_node = parseBlock();
			expect(nextToken(), Token::TYPE::CLOSE_BRAKET);
			return _node;
		case Token::TYPE::PRINT:
			return parsePrintStmt();
		default:
			return parseExpression();
		}

	}

	// varStmt ::= VAR ID (':' TYPE)? '=' exp [ , ID (':' TYPE)? '=' EXP ]
	Node* Parser::parseVarStmt()
	{
		expect(nextToken(), Token::TYPE::VAR);

		VarSubExprNode *_subExpr = nullptr;
		auto _var = make_node<VarStmtNode>();
		expect(lookahead, Token::TYPE::IDENTIFIER);

		auto id = make_node<IdentifierNode>(*nextToken()._literal);
		_subExpr = parseVarSubExpr(id);
		_var->children.push_back(_subExpr);

		while (match(Token::TYPE::COMMA))
		{
			_subExpr = parseVarSubExpr();
			_var->children.push_back(_subExpr);
		}

		return _var;
	}

	VarSubExprNode* Parser::parseVarSubExpr(IdentifierNode *_id)
	{
		auto ptr = make_node<VarSubExprNode>();

		if (_id == nullptr)
		{
			expect(lookahead, Token::TYPE::IDENTIFIER);
			_id = make_node<IdentifierNode>(*nextToken()._literal);
			ptr->varName = _id;
		}
		else
			ptr->varName = _id;

		if (match(Token::TYPE::SEMICOLON))
		{
			nextToken();
			expect(lookahead, Token::TYPE::IDENTIFIER);
			ptr->typeName = make_node<IdentifierNode>(*nextToken()._literal);
			if (match(Token::TYPE::ASSIGN))
				ptr->expression = parseExpression();
		}
		else if (match(Token::TYPE::ASSIGN))
			ptr->expression = parseExpression();
		else
			ReportError("Expect semicolon or '='.");

		return ptr;
	}

	Node* Parser::parseExpression()
	{
		// Expression ::=
		//		Assignment | BinaryExpr
		// 
		// Assignment ::= Variable '=' Expression
		//
		// BinaryExpr :: BinaryExpr OP BinaryExpr | UnaryExpr
		//
		// Variable ::= ID ( '[' Expression ']' )?
		//
		// UnaryExpr ::= ('+' | '-' | '!') UnaryExpr | '(' Expression ')' | FuncCall
		//
		// FuncCall ::= ID '(' FuncCallArgs ')'
		if (match(Token::TYPE::IDENTIFIER))
		{
			Token _tk = nextToken();
			auto _id = make_node<IdentifierNode>(*_tk._literal);
			if (match(Token::TYPE::OPEN_PAREN))
			{
				auto _func = parseFuncCall(_id);
				if (Token::isOperator(lookahead))
				{
					auto _op = nextToken();
					return parseBinaryExpr(_func, _op);
				}
				else
					return _func;
			}
			else if (match(Token::TYPE::ASSIGN))
				return parseAssignment(_id);
			else if (Token::isOperator(lookahead))
			{
				auto _tk = nextToken();
				return parseBinaryExpr(_id, _tk);
			}
			else return _id;
		}
		else
			return parseBinaryExpr();
	}

	Node* Parser::parseAssignment(IdentifierNode* _id)
	{
		// current version:
		// Assignment ::=
		//		VAR '=' Expression
		//
		// future version:
		// Assignment ::=
		//		VAR { ',' VAR } = Expression { ',' Expression }
		//
		if (!_id)
		{
			expect(Token::TYPE::IDENTIFIER);
			string _str = *lookahead._literal;
			nextToken();
			_id = make_node<IdentifierNode>(_str);
		}

		expect(nextToken(), Token::TYPE::ASSIGN);
		Node* _exp = parseExpression();
		return make_node<AssignmentNode>(_id, _exp);
	}

	Node* Parser::parseUnaryExpr(OperatorType _op)
	{
		// UnaryExpr ::= 
		//	( '+' | '-' | '!' ) UnaryExpr | Number | VAR | FUNCTIONCALL
		Node *_node = nullptr,
			*exp = nullptr;
		Token _tk;
		std::string _id_name;
		switch (lookahead.type)
		{
		case Token::TYPE::ADD:
			_node = parseUnaryExpr();
			break;
		case Token::TYPE::SUB:
		case Token::TYPE::NOT:
			_tk = nextToken();
			_node = make_node<UnaryExprNode>(Token::toOperator(_tk), parseUnaryExpr());
			break;
		case Token::TYPE::NUMBER:
			_node = make_node<NumberNode>(lookahead._double, lookahead.maybeInt);
			nextToken();
			break;
		case Token::TYPE::STRING:
			_id_name = *nextToken()._literal;
			_node = make_node<StringNode>(_id_name);
			break;
		case Token::TYPE::IDENTIFIER:
			_id_name = *nextToken()._literal;
			_node = make_node<IdentifierNode>(_id_name);

			if (match(Token::TYPE::OPEN_PAREN))
				_node = parseFuncCall(_node);
			break;
		case Token::TYPE::OPEN_PAREN:
			nextToken();
			exp = parseExpression();
			expect(nextToken(), Token::TYPE::CLOSE_PAREN);
			if (match(Token::TYPE::OPEN_PAREN))
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
				nextToken();
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
						exp = make_node<BinaryExprNode>(left_op, left_exp, exp);
						quit = true;
					}

					if (!quit)
						exp = make_node<NumberNode>(result, _maybeInt);
				}
				else
					exp = make_node<BinaryExprNode>(left_op, left_exp, exp);
				nextToken();
				return parseBinaryExpr(exp, right_tk);
			}
			else // left_op > right_op
			{
				if (left_exp)
					exp = make_node<BinaryExprNode>(left_op, left_exp, exp);
				return exp;
			}
		}
	}

	Node* Parser::parseIfStmt()
	{
		expect(nextToken(), Token::TYPE::IF);
		expect(nextToken(), Token::TYPE::OPEN_PAREN);
		auto _condition = parseBinaryExpr();
		expect(nextToken(), Token::TYPE::CLOSE_PAREN);

		auto _stmt = parseStatement();
		CHECK_NULL(_stmt)

		Node* _else = nullptr;
		if (match(Token::TYPE::ELSE))
			_else = parseElseStmt();
		return make_node<IfStmtNode>(_condition, _stmt, _else);
	}

	Node* Parser::parseElseStmt()
	{
		expect(nextToken(), Token::TYPE::ELSE);
		if (match(Token::TYPE::IF))
			return parseIfStmt();

		auto _stmt = parseStatement();
		return _stmt;
	}

	Node* Parser::parseWhileStmt()
	{
		expect(nextToken(), Token::TYPE::WHILE);
		expect(nextToken(), Token::TYPE::OPEN_PAREN); // (
		auto _condition = parseBinaryExpr();
		expect(nextToken(), Token::TYPE::CLOSE_PAREN);
		auto _stmt = parseStatement();
		return make_node<WhileStmtNode>(_condition, _stmt);
	}

	Node* Parser::parseFuncDef()
	{
		// def new function
		auto _func = make_node<FuncDefNode>();
		expect(nextToken(), Token::TYPE::FUNC);
		if (match(Token::TYPE::IDENTIFIER))
			_func->name = make_node<IdentifierNode>(*nextToken()._literal);
		else
			_func->name = nullptr;
		expect(nextToken(), Token::TYPE::OPEN_PAREN);
		while (match(Token::TYPE::IDENTIFIER))
		{
			auto param = reinterpret_cast<FuncDefParamNode*>(parseFuncDefParam());
			_func->parameters.push_back(param);
			if (match(Token::TYPE::COMMA))
				nextToken();
			else if (match(Token::TYPE::CLOSE_PAREN))
				break;
			else
				ReportError("Unexpected def params.");
		}
		expect(nextToken(), Token::TYPE::CLOSE_PAREN);
		// type
		if (match(Token::TYPE::SEMICOLON))
		{
			nextToken();
			_func->typeName = make_node<IdentifierNode>(*nextToken()._literal);
		}
		expect(nextToken(), Token::TYPE::OPEN_BRAKET);
		_func->block = reinterpret_cast<BlockExprNode*>(parseBlock());
		expect(nextToken(), Token::TYPE::CLOSE_BRAKET);
		return _func;
	}

	Node* Parser::parseFuncDefParam()
	{
		auto param = make_node<FuncDefParamNode>();
		expect(lookahead, Token::TYPE::IDENTIFIER);
		param->name = *nextToken()._literal;
		if (match(Token::TYPE::SEMICOLON))
		{
			nextToken();
			expect(Token::TYPE::IDENTIFIER);
			param->typeName = *nextToken()._literal;
		}
		return param;
	}

	Node* Parser::parseFuncCall(Node* exp)
	{
		FuncCallNode* _node = make_node<FuncCallNode>(exp);
		// FuncCallParamNode* _params = nullptr;

		expect(nextToken(), Token::TYPE::OPEN_PAREN);
		while (!expect(lookahead, Token::TYPE::CLOSE_PAREN))
		{
			auto node = parseExpression();

			_node->parameters.push_back(node);
			if (match(Token::TYPE::COMMA))
				continue;
			else if (match(Token::TYPE::CLOSE_PAREN))
				break;
			else
				ReportError("Expect identifier or comma.");
		}
		expect(nextToken(), Token::TYPE::CLOSE_PAREN);

		while (match(Token::TYPE::OPEN_PAREN))
		{
			nextToken();
			_node = make_node<FuncCallNode>(_node);

			while (!expect(lookahead, Token::TYPE::CLOSE_PAREN))
			{
				auto node = parseExpression();

				_node->parameters.push_back(node);
				if (match(Token::TYPE::COMMA))
					continue;
				else if (match(Token::TYPE::CLOSE_PAREN))
					break;
				else
					ReportError("Expect identifier or comma.");
			}

			expect(nextToken(), Token::TYPE::CLOSE_PAREN);
		}
		return _node;
	}

	Node* Parser::parseReturnStmt()
	{
		expect(nextToken(), Token::TYPE::RETURN);
		auto exp = parseExpression();
		return make_node<ReturnStmtNode>(exp);
	}

	Node* Parser::parsePrintStmt()
	{
		expect(nextToken(), Token::TYPE::PRINT);
		auto exp = parseExpression();
		return make_node<PrintStmtNode>(exp);
	}

	Parser::~Parser()
	{
		for (auto i = _node_list.begin(); i != _node_list.end(); ++i)
			delete *i;
	}

}
