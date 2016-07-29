#include "stdafx.h"
#include "codegen.h"
#include "svm_codes.h"

namespace codegen
{

	using namespace parser;

	int Environment::findLocalVariable(std::string _name, bool created)
	{
		if (var_id.find(_name) == var_id.end()) // not found
		{
			if (created)
			{
				auto id = var_id.size();
				var_id[_name] = id;
				return id;
			}
			else
				return -1;
		}
		else
			return var_id[_name];
	}

	CodeGen::CodeGen(Parser& _p) : _parser(_p)
	{ }

	void CodeGen::generate()
	{
		env = new Environment;

		visit(_parser.getRoot());

		env->instructions.push_back(Instruction(VM_CODE::STOP, 0));
		env->var_size = env->var_id.size();

		// clear Environment
		Environment* envPtr = env;
		while (envPtr)
		{
			auto _parent = envPtr->parent;
			delete envPtr;
			envPtr = _parent;
		}
		envPtr = nullptr;
	}

	void CodeGen::visit(Node* _node)
	{
		if (_node == nullptr)
			return;
		else if (_node->asIdentifier())
			visit(_node->asIdentifier());
		else if (_node->asNumber())
			visit(_node->asNumber());
		else if (_node->asAssignment())
			visit(_node->asAssignment());
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
		else if (_node->asBreakStmt())
			visit(_node->asBreakStmt());
		else if (_node->asReturnStmt())
			visit(_node->asReturnStmt());
		else if (_node->asFuncDef())
			visit(_node->asFuncDef());
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
				env->instructions.push_back(Instruction(VM_CODE::OUT, 0));
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
			env->instructions.push_back(Instruction(VM_CODE::ADD, 0));
			break;
		case OperatorType::SUB:
			env->instructions.push_back(Instruction(VM_CODE::SUB, 0));
			break;
		case OperatorType::MUL:
			env->instructions.push_back(Instruction(VM_CODE::MUL, 0));
			break;
		case OperatorType::DIV:
			env->instructions.push_back(Instruction(VM_CODE::DIV, 0));
			break;
		case OperatorType::MOD:
			env->instructions.push_back(Instruction(VM_CODE::MOD, 0));
			break;
		case OperatorType::POW:
			env->instructions.push_back(Instruction(VM_CODE::POW, 0));
			break;
		case OperatorType::GT:
			env->instructions.push_back(Instruction(VM_CODE::GT, 0));
			break;
		case OperatorType::LT:
			env->instructions.push_back(Instruction(VM_CODE::LT, 0));
			break;
		case OperatorType::GTEQ:
			env->instructions.push_back(Instruction(VM_CODE::GTEQ, 0));
			break;
		case OperatorType::LTEQ:
			env->instructions.push_back(Instruction(VM_CODE::LTEQ, 0));
			break;
		case OperatorType::EQ:
			env->instructions.push_back(Instruction(VM_CODE::EQ, 0));
			break;
		default:
			// runtime error
			env->instructions.push_back(Instruction(VM_CODE::POP, 0));
			// pack.instructions.push_back(Instruction(VM_CODE::POP, 0));
		}
	}

	void CodeGen::visit(NumberNode* _node)
	{
		auto index = env->constant.size();

		if (_node->maybeInt)
			env->constant.push_back(Object(static_cast<int>(_node->number)));
		env->constant.push_back(Object(_node->number));

		env->instructions.push_back(Instruction(VM_CODE::LOAD_C, index));
	}

	void CodeGen::visit(IdentifierNode* _node)
	{
		// read the value of the memory
		// and push to the top of the stack

		auto index = env->findLocalVariable(_node->name, true);
		env->instructions.push_back(Instruction(VM_CODE::LOAD_V, index));
	}

	void CodeGen::visit(AssignmentNode* _node)
	{
		// find if the var is exisits
		// if not exisits add a possition for it
		auto _id = dynamic_cast<IdentifierNode*>(_node->identifier);
		unsigned int _index;
		if (env->var_id.find(_id->name) == env->var_id.end()) // not find
		{
			_index = env->var_id.size();
			env->var_id[_id->name] = _index;
			env->var_names[_index] = _id->name;	// record the name
		}
		else
			_index = env->var_id[_id->name];

		visit(_node->expression);
		env->instructions.push_back(Instruction(VM_CODE::STORE_V, _index));
	}

	void CodeGen::visit(IfStmtNode* _node)
	{
		int jmp_val;
		visit(_node->condition);
		auto jmp_loc = env->instructions.size();
		env->instructions.push_back(Instruction(VM_CODE::IFNO, 1));
		visit(_node->true_branch);
		auto true_finish_loc = env->instructions.size();
		env->instructions.push_back(Instruction(VM_CODE::JMP, 1));
		// if condition not ture, jmp to the right location
		jmp_val = env->instructions.size() - jmp_loc;
		env->instructions[jmp_loc] = Instruction(VM_CODE::IFNO, jmp_val); 
		if (_node->false_branch)
		{
			visit(_node->false_branch);
			jmp_val = env->instructions.size() - true_finish_loc;
			env->instructions[true_finish_loc] = Instruction(VM_CODE::JMP, jmp_val);
		}
	}

	void CodeGen::visit(WhileStmtNode* _node)
	{
		auto _begin_loc = env->instructions.size();
		visit(_node->condition);
		auto _condition_loc = env->instructions.size();
		env->instructions.push_back(Instruction(VM_CODE::IFNO, 0));
		visit(_node->child);
		env->instructions.push_back(Instruction(VM_CODE::JMP, -1 * (env->instructions.size() - _begin_loc)));
		env->instructions[_condition_loc] = Instruction(VM_CODE::IFNO, env->instructions.size() - _condition_loc);
	}

	void CodeGen::visit(BreakStmtNode* _node)
	{

	}

	void CodeGen::visit(ReturnStmtNode* _node)
	{

	}

	void CodeGen::visit(FuncDefNode* _node)
	{
		// wait to finish
	}

}
