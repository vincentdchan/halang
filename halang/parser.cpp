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

	/**********************************/
	/*   Paring                       */
	/**********************************/

	// Program -> [ Statement ]
	void Parser::ParseProgram()
	{
		StartNode();
		auto _node = MakeObject<ProgramNode>();

		while (!Match(Token::TYPE::ENDFILE)) {
			_node->statements.push_back(
				ParseStatement()
			);
			if (!ok) return;
		}

		ast_root = reinterpret_cast<ProgramNode*>(
			FinishNode(_node)
		);
	}

	Node* Parser::ParseStatement()
	{
		switch (current_tok->type)
		{
		case Token::TYPE::SEMICOLON:
			NextToken();
			return nullptr;
		case Token::TYPE::LET:
			return ParseLetStatement();
		case Token::TYPE::WHILE:
			return ParseWhileStatement();
		case Token::TYPE::BREAK:
			StartNode();
			NextToken();
			return FinishNode(
				MakeObject<BreakStatementNode>()
			);
		case Token::TYPE::IF:
			return ParseIfStatement();
		case Token::TYPE::CONTINUE:
			StartNode();
			NextToken();
			return FinishNode(
				MakeObject<ContinueStatementNode>()
			);
		case Token::TYPE::RETURN:
			return ParseReturnStatement();
		case Token::TYPE::DEF:
			return ParseDefStatement();
		case Token::TYPE::IDENTIFIER:
			return ParseCallExpression();
		default:
			AddError("Unexpected token");
		}

	}

	Node* Parser::ParseLetStatement() {
		StartNode();
		Expect(Token::TYPE::LET);
		NextToken();
		auto _node = MakeObject<LetStatementNode>();

		Expect(Token::TYPE::IDENTIFIER);

		CHECK_OK
		while (Match(Token::TYPE::IDENTIFIER)) {
			StartNode();
			Node* sub_node;

			auto id = NextToken();
			if (Match(Token::TYPE::ASSIGN)) {
				sub_node = ParseAssignExpression(
					MakeObject<IdentifierNode>(id->GetLiteralValue())
				);
				CHECK_OK
			} else {
				sub_node = FinishNode(
					MakeObject<IdentifierNode>(id->GetLiteralValue())
				);
			}

			_node->assignments.push_back(sub_node);

			if (Match(Token::TYPE::COMMA)) {
				NextToken();
				Expect(Token::TYPE::IDENTIFIER);
			} else {
				break;
			}

		}

		return FinishNode(_node);
	}

	Node* Parser::ParseIfStatement() {
		Expect(Token::TYPE::IF);
		CHECK_OK

		auto _node = MakeObject<IfStatementNode>();

		StartNode();
		NextToken();

		auto _cond = ParseExpression();
		CHECK_OK

		Expect(Token::TYPE::THEN);
		NextToken();

		while (!Match(Token::TYPE::ELSE) && 
		!Match(Token::TYPE::END)) {
			auto _sub_stat = ParseStatement();
			_node->children.push_back(_sub_stat);

			CHECK_OK
		}

		if (Match(Token::TYPE::ELSE)) {
			NextToken();

			if (Match(Token::TYPE::IF)) {
				auto _sub_if = ParseIfStatement();
				CHECK_OK
				_node->else_children.push_back(_sub_if);
			} else if (Match(Token::TYPE::END)) {
				NextToken();
			} else {
				while (!Match(Token::TYPE::END)) {
					auto stat = ParseStatement();
					_node->else_children.push_back(stat);
					CHECK_OK
				}

				Expect(Token::TYPE::END);
				NextToken();
			}
		} else {
			Expect(Token::TYPE::END);
			NextToken();
		}


		_node->condition = _cond;

		return FinishNode(_node);
	}

	Node* Parser::ParseWhileStatement() {
		Expect(Token::TYPE::WHILE);
		CHECK_OK
		StartNode();

		NextToken();
		auto _node = MakeObject<WhileStatementNode>();

		_node->condition = ParseExpression();
		CHECK_OK

		Expect(Token::TYPE::DO);
		NextToken();
		CHECK_OK

		while (!Match(Token::TYPE::END)) {
			auto _stat = ParseStatement();
			CHECK_OK
			_node->children.push_back(_stat);
		}

		Expect(Token::TYPE::END);
		NextToken();

		return FinishNode(_node);
	}

	Node* Parser::ParseReturnStatement() {
		Expect(Token::TYPE::RETURN);
		CHECK_OK
		StartNode();

		auto _node = MakeObject<ReturnStatementNode>();
		NextToken();

		if (
			Token::IsOperator(*current_tok) || 
			Match(Token::TYPE::NUMBER) ||
			Match(Token::TYPE::IDENTIFIER) ||
			Match(Token::TYPE::STRING) ||
			Match(Token::TYPE::DO) ||
			Match(Token::TYPE::FUN)
		) {
			_node->expression = ParseExpression();
		}
		CHECK_OK

		return FinishNode(_node);
	}

	Node* Parser::ParseDefStatement() {
		Expect(Token::TYPE::DEF);
		CHECK_OK
		StartNode();
		NextToken();

		auto _node = MakeObject<DefStatementNode>();

		Expect(Token::TYPE::IDENTIFIER);
		CHECK_OK
		_node->name = reinterpret_cast<IdentifierNode*>(
			ParseIdentifier()
		); 
   
		Expect(Token::TYPE::OPEN_PAREN);
		NextToken();

		while (!Match(Token::TYPE::CLOSE_PAREN)) {
			auto param = ParseExpression();
			CHECK_OK

			_node->params.push_back(param);
			if (Match(Token::TYPE::COMMA)) {
				NextToken();
			} else {
				Expect(Token::TYPE::CLOSE_PAREN);
				CHECK_OK
				break;
			}
		}

		Expect(Token::TYPE::CLOSE_PAREN);
		NextToken();
		CHECK_OK

		while (!Match(Token::TYPE::END)) {
			auto stat = ParseStatement();
			_node->body.push_back(stat);
		}

		Expect(Token::TYPE::END);
		NextToken();

		return FinishNode(_node);
	}

	Node* Parser::ParseExpression()
	{
		auto _left = ParseMaybeUnary();
		return _left;
	}

	Node* Parser::ParseMaybeUnary()
	{
		if (Token::IsOperator(*current_tok)) {
			StartNode();

			auto _node = MakeObject<UnaryExpressionNode>();
			_node->op = Token::ToOperator(*current_tok);
			NextToken();
			_node->child = ParseMaybeUnary();

			return FinishNode(_node);
		}
	}

	Node* Parser::ParseExpressionUnit() {
		if (Match(Token::TYPE::OPEN_PAREN)) {
			NextToken();
			auto _node = ParseExpression();
			Expect(Token::TYPE::CLOSE_PAREN);
			NextToken();

			CHECK_OK
			return _node;
		} else if (Match(Token::TYPE::IDENTIFIER)) {
			return ParseMaybeCallExpression();
		} else if (Match(Token::TYPE::OPEN_SQUARE_BRACKET)) {
			return ParseListExpression();
		} else if (Match(Token::TYPE::NUMBER)) {
			StartNode();
			auto _node = MakeObject<NumberNode>();
			_node->number = current_tok->GetDoubleValue();
			_node->maybeInt = current_tok->maybeInt;
			NextToken();
			return FinishNode(_node);
		} else if (Match(Token::TYPE::STRING)) {
			StartNode();
			auto _node = MakeObject<StringNode>();
			_node->content = current_tok->GetLiteralValue();
			NextToken();
			return FinishNode(_node);
		} else {
			AddError("Unexpected expression unit");
		}
	}

	Node* Parser::ParseAssignExpression(IdentifierNode* _id)
	{
		// StartNode();
		// if (!_id)
		// {
		// 	Expect(Token::TYPE::IDENTIFIER);
		// 	std::u16string _str = *lookahead._literal;
		// 	NextToken();
		// 	_id = MakeObject<IdentifierNode>(_str);
		// }

		// Expect(NextToken(), Token::TYPE::ASSIGN);
		// Node* _exp = parseExpression();

		// CHECK_OK

		// return FinishNode(
		// 	MakeObject<AssignmentNode>(_id, _exp)
		// );
		return nullptr;
	}

	Node* Parser::ParseListExpression()
	{
		StartNode();
		auto list = MakeObject<ListExpressionNode>();

		Expect(Token::TYPE::OPEN_SQUARE_BRACKET);
		NextToken();
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

			NextToken();
			Expect(Token::TYPE::CLOSE_SQUARE_BRACKET);
		}
		else // ']'
			NextToken();
		
		CHECK_OK

		return FinishNode(list);
	}

	Node* Parser::ParseBinaryExpr(Node* left_exp, Token* left_tk)
	{
		// StartNode();

		// Node* exp = ParseUnaryExpr();
		// CHECK_NULL(exp);
		// CHECK_OK
		// OperatorType left_op = Token::ToOperator(*left_tk);
		// while (true)
		// {
		// 	Token* right_tk = current_tok;
		// 	OperatorType right_op = Token::ToOperator(*right_tk);
		// 	if (getPrecedence(left_op) < getPrecedence(right_op))
		// 	{
		// 		NextToken();
		// 		exp = ParseBinaryExpr(exp, right_tk);
		// 		CHECK_OK
		// 	}
		// 	else if (getPrecedence(left_op) == getPrecedence(right_op))
		// 	{
		// 		if (getPrecedence(left_op) == 0)
		// 			return exp;
		// 		CHECK_NULL(left_exp);
		// 		if (left_exp->AsNumber() && exp->AsNumber())
		// 		{
		// 			auto num1_exp = left_exp->AsNumber();
		// 			auto num2_exp = exp->AsNumber();
		// 			auto num1 = num1_exp->number;
		// 			auto num2 = num2_exp->number;

		// 			double result = 0;
		// 			auto _maybeInt = num1_exp->maybeInt && num2_exp->maybeInt;

		// 			bool quit = false;
		// 			switch (left_op)
		// 			{
		// 			case OperatorType::ADD:
		// 				result = num1 + num2;
		// 				break;
		// 			case OperatorType::SUB:
		// 				result = num1 - num2;
		// 				break;
		// 			case OperatorType::MUL:
		// 				result = num1 * num2;
		// 				break;
		// 			case OperatorType::DIV:
		// 				result = num1 / num2;
		// 				break;
		// 			case OperatorType::MOD:
		// 				result = static_cast<int>(num1) & static_cast<int>(num2);
		// 				break;
		// 			case OperatorType::POW:
		// 				result = pow(num1, num2);
		// 				break;
		// 			default:
		// 				exp = MakeObject<BinaryExpressionNode>(
		// 					left_op, 
		// 					left_exp, 
		// 					exp
		// 				);
		// 				quit = true;
		// 			}

		// 			if (!quit)
		// 				exp = MakeObject<NumberNode>(result, _maybeInt);
		// 		}
		// 		else
		// 			exp = MakeObject<BinaryExpressionNode>(
		// 				left_op, 
		// 				left_exp, 
		// 				exp
		// 			);
		// 		NextToken();
		// 		return FinishNode(ParseBinaryExpr(exp, right_tk));
		// 	}
		// 	else // left_op > right_op
		// 	{
		// 		if (left_exp)
		// 			exp = MakeObject<BinaryExpressionNode>(left_op, left_exp, exp);
		// 		return FinishNode(exp);
		// 	}
		// }
		return nullptr;
	}

	// Node* Parser::ParseFuncCall(Node* exp)
	// {
	// 	StartNode();
	// 	FuncCallNode* _node = MakeObject<FuncCallNode>(exp);
	// 	// FuncCallParamNode* _params = nullptr;

	// 	Expect(, Token::TYPE::OPEN_PAREN);
	// 	NextToken();
	// 	while (!Match(Token::TYPE::CLOSE_PAREN))
	// 	{
	// 		auto node = ParseExpression();
	// 		CHECK_OK

	// 		_node->parameters.push_back(node);
	// 		if (Match(Token::TYPE::COMMA))
	// 			NextToken();
	// 		else if (Match(Token::TYPE::CLOSE_PAREN))
	// 			break;
	// 		else
	// 			AddError("Expect identifier or comma.");
	// 	}
	// 	Expect(NextToken(), Token::TYPE::CLOSE_PAREN);

	// 	while (Match(Token::TYPE::OPEN_PAREN))
	// 	{
	// 		NextToken();
	// 		_node = MakeObject<FuncCallNode>(_node);

	// 		while (!Expect(Token::TYPE::CLOSE_PAREN))
	// 		{
	// 			auto node = ParseExpression();
	// 			CHECK_OK

	// 			_node->parameters.push_back(node);
	// 			if (Match(Token::TYPE::COMMA))
	// 				continue;
	// 			else if (Match(Token::TYPE::CLOSE_PAREN))
	// 				break;
	// 			else
	// 				ReportError("Expect identifier or comma.");
	// 		}

	// 		Expect(NextToken(), Token::TYPE::CLOSE_PAREN);
	// 	}
	// 	return FinishNode(_node);
	// }

	Node* Parser::ParseCallExpression(Node* src) {
		Expect(Token::TYPE::OPEN_PAREN);
		StartNode();
		NextToken();

		auto* _node = MakeObject<CallExpressionNode>();
		_node->callee = src;

		// parse params

		Expect(Token::TYPE::CLOSE_PAREN);
		NextToken();

		Node *result = FinishNode(_node);

		if (Match(Token::TYPE::OPEN_PAREN)) {
			return ParseCallExpression(result);
		} else if (Match(Token::TYPE::OPEN_SQUARE_BRACKET)) {
			return ParseMemberExpression(result);
		} else {
			return result;
		}
	}

	Node* Parser::ParseMemberExpression(Node* src) {
		Expect(Token::TYPE::OPEN_SQUARE_BRACKET);
		StartNode();
		NextToken();

		auto _node = MakeObject<MemberExpressionNode>();
		_node->left = src;
		_node->right = ParseExpression();

		CHECK_OK

		Expect(Token::TYPE::CLOSE_SQUARE_BRACKET);
		NextToken();

		Node *result = FinishNode(_node);

		if (Match(Token::TYPE::OPEN_PAREN)) {
			return ParseCallExpression(result);
		} else if (Match(Token::TYPE::OPEN_SQUARE_BRACKET)) {
			return ParseMemberExpression(result);
		} else {
			return result;
		}
	}

	Node* Parser::ParseIdentifier() {
		Expect(Token::TYPE::IDENTIFIER);
		StartNode();
		auto _node = MakeObject<IdentifierNode>(
			current_tok->GetLiteralValue()
		);
		NextToken();
		return FinishNode(_node);
	}

	Node* Parser::ParseMaybeCallExpression() {
		return nullptr;
	}

	Parser::~Parser()
	{
	}

}
