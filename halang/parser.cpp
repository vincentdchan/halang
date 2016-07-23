
#include "stdafx.h"
#include "parser.h"
#include "ast.h"

namespace parser
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
			_block->children.push_back(parseStatement());
		} while (match(Token::TYPE::IDENTIFIER) ||
			match(Token::TYPE::NUMBER) ||
			match(Token::TYPE::LET) ||
			match(Token::TYPE::OPEN_PAREN) ||
			match(Token::TYPE::WHILE) ||
			Token::isOperator(lookahead)
			);

		return _block;
	}

	Node* Parser::parseStatement()
	{
		if (match(Token::TYPE::LET))
			return parseLetExpr();
		else if (match(Token::TYPE::WHILE))
			return parseWhileStmt();
		else
			return parseExpression();
	}

	Node* Parser::parseExpression()
	{
		return parseBinaryExpr();
	}

	Node* Parser::parseLetExpr()
	{
		expect(Token::TYPE::LET);
		nextToken();
		return parseAssignment();
	}

	Node* Parser::parseAssignment()
	{
		expect(Token::TYPE::IDENTIFIER);

		string _id = *lookahead._literal;
		nextToken();
		expect(Token::TYPE::ASSIGN);
		nextToken();
		Node* _exp = parseBinaryExpr();
		return make_node<AssignmentNode>(_id, _exp);
	}

	Node* Parser::parseUnaryExpr()
	{
		// UnaryExpr: ( + | - )? Int
		if (match(Token::TYPE::ADD))
		{
			nextToken();
			if (match(Token::TYPE::NUMBER))
			{
				double _data = lookahead._double;
				auto numberNode = make_node<NumberNode>(_data, lookahead.maybeInt);
				nextToken();
				return numberNode;
			}
			else if (match(Token::TYPE::IDENTIFIER))
			{
				string s = *lookahead._literal;
				nextToken();
				return make_node<IdentifierNode>(s);
			}
			else
			{
				ReportError("Expected number.");
				return nullptr;
			}
		}
		else if (match(Token::TYPE::SUB))
		{
			nextToken();
			if (match(Token::Token::NUMBER))
			{
				double _data = lookahead._double;
				auto _node = make_node<NumberNode>(_data, lookahead.maybeInt);
				nextToken();
				return _node;
			}
			else if (match(Token::TYPE::IDENTIFIER))
			{
				Token _t = nextToken();
				auto _node = make_node<IdentifierNode>(*_t._literal);
				return make_node<UnaryExprNode>(OperatorType::SUB, _node);
			}
			else
			{
				ReportError("Expected number or identifier");
				return nullptr;
			}
		}
		else if (match(Token::TYPE::NUMBER))
		{
			Token _t = nextToken();
			return make_node<NumberNode>(_t._double, _t.maybeInt);
		}
		else if (match(Token::TYPE::IDENTIFIER))
		{
			Token _t = nextToken();
			return make_node<IdentifierNode>(*_t._literal);
		}
		else if (match(Token::TYPE::OPEN_PAREN))
		{
			nextToken();
			auto _node = parseBinaryExpr();
			expect(Token::TYPE::CLOSE_PAREN);
			nextToken();
			return _node;
		}
		else
		{
			ReportError("Unexpected token, expected +, - or Integer.");
			return nullptr;
		}
	}

	Node* Parser::parseBinaryExpr()
	{
		// BinaryExpr: Unary { OP Unary }
		stack<OperatorType> opStack;
		stack<Node*> nodeStack;
		auto _begin = parseUnaryExpr();
		if (_begin == nullptr)
			return nullptr;
		nodeStack.push(_begin);

		while (Token::isOperator(lookahead))
		{
			int prec = getPrecedence(Token::toOperator(lookahead));
			if (opStack.empty() || prec > getPrecedence(opStack.top()))
			{
				opStack.push(Token::toOperator(lookahead));
				nextToken();
				auto l = parseUnaryExpr();
				nodeStack.push(l);
			}
			else
			{
				while (!opStack.empty() && prec <= getPrecedence(opStack.top()))
				{
					Node *l1, *l2;
					l1 = nodeStack.top(); nodeStack.pop();
					l2 = nodeStack.top(); nodeStack.pop();
					nodeStack.push(make_node<BinaryExprNode>(opStack.top(), l2, l1));
					opStack.pop();
				}
			}
		}
		while (!opStack.empty())
		{
			Node *l1, *l2;
			l1 = nodeStack.top(); nodeStack.pop();
			l2 = nodeStack.top(); nodeStack.pop();
			nodeStack.push(make_node<BinaryExprNode>(opStack.top(), l2, l1));
			opStack.pop();
		}
		return nodeStack.top();
	}

	Node* Parser::parseIfStmt()
	{
		expect(nextToken(), Token::TYPE::IF);
		expect(nextToken(), Token::TYPE::OPEN_PAREN);
		auto _condition = parseBinaryExpr();
		expect(nextToken(), Token::TYPE::CLOSE_PAREN);
		expect(nextToken(), Token::TYPE::OPEN_BRAKET);
		auto _block = parseBlock();
		expect(nextToken(), Token::TYPE::CLOSE_BRAKET);
		Node* _else = nullptr;
		if (match(Token::TYPE::ELSE))
			_else = parseElseStmt();
		return make_node<IfStmtNode>(_condition, _block, _else);
	}

	Node* Parser::parseElseStmt()
	{
		expect(nextToken(), Token::TYPE::ELSE);
		if (match(Token::TYPE::IF))
			return parseIfStmt();
		expect(nextToken(), Token::TYPE::OPEN_BRAKET);
		auto _block = parseBlock();
		expect(nextToken(), Token::TYPE::CLOSE_BRAKET);
		return _block;
	}

	Node* Parser::parseWhileStmt()
	{
		expect(nextToken(), Token::TYPE::WHILE);
		expect(nextToken(), Token::TYPE::OPEN_PAREN); // (
		auto _condition = parseBinaryExpr();
		expect(nextToken(), Token::TYPE::CLOSE_PAREN);
		expect(nextToken(), Token::TYPE::OPEN_BRAKET);
		auto _block = parseBlock();
		expect(nextToken(), Token::TYPE::CLOSE_BRAKET);
		return make_node<WhileStmtNode>(_condition, _block);
	}

	Parser::~Parser()
	{
		for (auto i = _node_list.begin(); i != _node_list.end(); ++i)
			delete *i;
	}

}
