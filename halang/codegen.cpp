#include "stdafx.h"
#include "codegen.h"
#include "svm_codes.h"

namespace codegen
{

	using namespace parser;

	CodeGen::CodeGen(Parser& _p) : _parser(_p)
	{ }

	void CodeGen::generate()
	{
		visit(_parser.getRoot());
		pack.instructions.push_back(Instruction(VM_CODE::STOP, 0));
		pack.variablesSize = var_id.size();
	}

	void CodeGen::visit(Node* _node)
	{
		if (_node->asIdentifier())
			visit(_node->asIdentifier());
		else if (_node->asAssignment())
			visit(_node->asAssignment());
		else if (_node->asNumber())
			visit(_node->asNumber());
		else if (_node->asBinaryExpression())
			visit(_node->asBinaryExpression());
		else if (_node->asUnaryExpression())
			visit(_node->asUnaryExpression());
		else if (_node->asBlockExpression())
			visit(_node->asBlockExpression());
		else if (_node->asIfStmt())
			visit(_node->asIfStmt());
		else if (_node->asWhileStmt())
			visit(_node->asWhileStmt());
		else if (_node->asDefStmt())
			visit(_node->asDefStmt());
	}

	void CodeGen::visit(BlockExprNode* _node)
	{
		for (auto i = _node->children.begin(); i != _node->children.end(); ++i)
		{
			visit(*i);
			if ((*i)->asUnaryExpression() || 
				(*i)->asBinaryExpression() || 
				(*i)->asIdentifier() || 
				(*i)->asNumber())
				pack.instructions.push_back(Instruction(VM_CODE::OUT, 0));
		}
	}

	void CodeGen::visit(UnaryExprNode* _node)
	{

	}

	void CodeGen::visit(BinaryExprNode* _node)
	{
		visit(_node->left);
		visit(_node->right);
		switch (_node->op)
		{
		case OperatorType::ADD:
			pack.instructions.push_back(Instruction(VM_CODE::ADD, 0));
			break;
		case OperatorType::SUB:
			pack.instructions.push_back(Instruction(VM_CODE::SUB, 0));
			break;
		case OperatorType::MUL:
			pack.instructions.push_back(Instruction(VM_CODE::MUL, 0));
			break;
		case OperatorType::DIV:
			pack.instructions.push_back(Instruction(VM_CODE::DIV, 0));
			break;
		case OperatorType::MOD:
			pack.instructions.push_back(Instruction(VM_CODE::MOD, 0));
			break;
		case OperatorType::POW:
			pack.instructions.push_back(Instruction(VM_CODE::POW, 0));
			break;
		case OperatorType::GT:
			pack.instructions.push_back(Instruction(VM_CODE::GT, 0));
			break;
		case OperatorType::LT:
			pack.instructions.push_back(Instruction(VM_CODE::LT, 0));
			break;
		case OperatorType::GTEQ:
			pack.instructions.push_back(Instruction(VM_CODE::GTEQ, 0));
			break;
		case OperatorType::LTEQ:
			pack.instructions.push_back(Instruction(VM_CODE::LTEQ, 0));
			break;
		case OperatorType::EQ:
			pack.instructions.push_back(Instruction(VM_CODE::EQ, 0));
			break;
		default:
			// runtime error
			pack.instructions.push_back(Instruction(VM_CODE::POP, 0));
			// pack.instructions.push_back(Instruction(VM_CODE::POP, 0));
		}
	}

	void CodeGen::visit(NumberNode* _node)
	{
		auto index = pack.constant.size();
		if (_node->maybeInt)
			pack.constant.push_back(Object(static_cast<int>(_node->value)));
		else
			pack.constant.push_back(Object(_node->value));
		pack.instructions.push_back(Instruction(VM_CODE::LOAD_C, index));
	}

	void CodeGen::visit(IdentifierNode* _node)
	{
		// read the value of the memory
		// and push to the top of the stack
		auto index = var_id[_node->name];
		pack.instructions.push_back(Instruction(VM_CODE::LOAD_V, index));
	}

	void CodeGen::visit(AssignmentNode* _node)
	{
		// find if the var is exisits
		// if not exisits add a possition for it
		auto _name = _node->identifier;
		unsigned int _index;
		if (var_id.find(_name) == var_id.end()) // not find
		{
			_index = var_id.size();
			var_id[_name] = _index;
		}
		else
			_index = var_id[_name];

		visit(_node->expression);
		pack.instructions.push_back(Instruction(VM_CODE::STORE_V, _index));
	}

	void CodeGen::visit(IfStmtNode* _node)
	{
		int jmp_val;
		visit(_node->condition);
		auto jmp_loc = pack.instructions.size();
		pack.instructions.push_back(Instruction(VM_CODE::IFNO, 1));
		visit(_node->true_branch);
		auto true_finish_loc = pack.instructions.size();
		pack.instructions.push_back(Instruction(VM_CODE::JMP, 1));
		// if condition not ture, jmp to the right location
		jmp_val = pack.instructions.size() - jmp_loc;
		pack.instructions[jmp_loc] = Instruction(VM_CODE::IFNO, jmp_val); 
		if (_node->false_branch)
		{
			visit(_node->false_branch);
			jmp_val = pack.instructions.size() - true_finish_loc;
			pack.instructions[true_finish_loc] = Instruction(VM_CODE::JMP, jmp_val);
		}
	}

	void CodeGen::visit(WhileStmtNode* _node)
	{
		auto _begin_loc = pack.instructions.size();
		visit(_node->condition);
		auto _condition_loc = pack.instructions.size();
		pack.instructions.push_back(Instruction(VM_CODE::IFNO, 0));
		visit(_node->child);
		pack.instructions.push_back(Instruction(VM_CODE::JMP, -1 * (pack.instructions.size() - _begin_loc)));
		pack.instructions[_condition_loc] = Instruction(VM_CODE::IFNO, pack.instructions.size() - _condition_loc);
	}

	void CodeGen::visit(DefStmtNode* _node)
	{
		// wait to finish
	}

}
