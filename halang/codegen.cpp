#include "stdafx.h"
#include "codegen.h"
#include "svm_codes.h"

namespace halang
{

	CodeGen::CodeGen(StackVM* _vm) : 
		vm(_vm), parser(nullptr)
	{ 
		top_cp = global_cp = vm->make_gcobject<CodePack>();
	}

	void CodeGen::generate(Parser* p)
	{
		parser = p;

		generate(global_cp);
		global_cp->instructions.push_back(Instruction(VM_CODE::STOP, 0));
	}

	void CodeGen::generate(CodePack* cp)
	{
		visit(cp, parser->getRoot());
	}

	void CodeGen::visit(CodePack* cp, Node* _node)
	{
		_node->visit(this, cp);
	}

	void CodeGen::visit(CodePack* cp, BlockExprNode* _node)
	{
		for (auto i = _node->children.begin(); i != _node->children.end(); ++i)
		{
			(*i)->visit(this, cp);

			// TODO: debug
			if ((*i)->asUnaryExpression() || 
				(*i)->asBinaryExpression() || 
				(*i)->asIdentifier() || 
				(*i)->asNumber())
				cp->instructions.push_back(Instruction(VM_CODE::OUT, 0));
		}
	}

	void CodeGen::visit(CodePack* cp, UnaryExprNode* _node)
	{
		visit(cp, _node->child);
		switch (_node->op)
		{
		case OperatorType::SUB:
			cp->instructions.push_back(Instruction(VM_CODE::PUSH_INT, -1));
			cp->instructions.push_back(Instruction(VM_CODE::MUL, 0));
			break;
		case OperatorType::NOT:
			cp->instructions.push_back(Instruction(VM_CODE::NOT, 0));
			break;
		}
	}

	void CodeGen::visit(CodePack* cp, BinaryExprNode* _node)
	{
		visit(cp, _node->left);
		visit(cp, _node->right);
		switch (_node->op)
		{
		case OperatorType::ADD:
			cp->instructions.push_back(Instruction(VM_CODE::ADD, 0));
			break;
		case OperatorType::SUB:
			cp->instructions.push_back(Instruction(VM_CODE::SUB, 0));
			break;
		case OperatorType::MUL:
			cp->instructions.push_back(Instruction(VM_CODE::MUL, 0));
			break;
		case OperatorType::DIV:
			cp->instructions.push_back(Instruction(VM_CODE::DIV, 0));
			break;
		case OperatorType::MOD:
			cp->instructions.push_back(Instruction(VM_CODE::MOD, 0));
			break;
		case OperatorType::POW:
			cp->instructions.push_back(Instruction(VM_CODE::POW, 0));
			break;
		case OperatorType::GT:
			cp->instructions.push_back(Instruction(VM_CODE::GT, 0));
			break;
		case OperatorType::LT:
			cp->instructions.push_back(Instruction(VM_CODE::LT, 0));
			break;
		case OperatorType::GTEQ:
			cp->instructions.push_back(Instruction(VM_CODE::GTEQ, 0));
			break;
		case OperatorType::LTEQ:
			cp->instructions.push_back(Instruction(VM_CODE::LTEQ, 0));
			break;
		case OperatorType::EQ:
			cp->instructions.push_back(Instruction(VM_CODE::EQ, 0));
			break;
		default:
			// runtime error
			cp->instructions.push_back(Instruction(VM_CODE::POP, 0));
			// pack.instructions.push_back(Instruction(VM_CODE::POP, 0));
		}
	}

	void CodeGen::visit(CodePack* cp, NumberNode* _node)
	{
		auto index = cp->constant.size();

		if (_node->maybeInt)
			cp->constant.push_back(Object(static_cast<int>(_node->number)));
		cp->constant.push_back(Object(_node->number));

		cp->instructions.push_back(Instruction(VM_CODE::LOAD_C, index));
	}

	void CodeGen::visit(CodePack* cp, StringNode* _node)
	{
		auto index = cp->constant.size();
		cp->constant.push_back(Object(_node->content));

		cp->instructions.push_back(Instruction(VM_CODE::LOAD_C, index));
	}

	void CodeGen::visit(CodePack* cp, IdentifierNode* _node)
	{
		// read the value of the memory
		// and push to the top of the stack

		int id = cp->findVarId(_node->name);

		if (id >= 0)
			cp->instructions.push_back(Instruction(VM_CODE::LOAD_V, id));
		// else codegen error;
	}

	void CodeGen::visit(CodePack* cp, AssignmentNode* _node)
	{
		// find if the var is exisits
		// if not exisits add a possition for it
		auto _id_node = dynamic_cast<IdentifierNode*>(_node->identifier);

		int _id = cp->findVarId(_id_node->name);

		if (_id < 0)
		{
			_id = cp->var_size++;
			cp->var_names.push_back(_id_node->name);
		}

		visit(cp, _node->expression);
		cp->instructions.push_back(Instruction(VM_CODE::STORE_V, _id));
	}

	void CodeGen::visit(CodePack* cp, VarStmtNode* _node)
	{
		for (auto i = _node->children.begin(); i != _node->children.end(); ++i)
			visit(cp, *i);
	}

	void CodeGen::visit(CodePack* cp, IfStmtNode* _node)
	{
		int jmp_val;
		visit(cp, _node->condition);
		auto jmp_loc = cp->instructions.size();
		cp->instructions.push_back(Instruction(VM_CODE::IFNO, 1));
		visit(cp, _node->true_branch);
		auto true_finish_loc = cp->instructions.size();
		cp->instructions.push_back(Instruction(VM_CODE::JMP, 1));
		// if condition not ture, jmp to the right location
		jmp_val = cp->instructions.size() - jmp_loc;
		cp->instructions[jmp_loc] = Instruction(VM_CODE::IFNO, jmp_val); 
		if (_node->false_branch)
		{
			visit(cp, _node->false_branch);
			jmp_val = cp->instructions.size() - true_finish_loc;
			cp->instructions[true_finish_loc] = Instruction(VM_CODE::JMP, jmp_val);
		}
	}

	void CodeGen::visit(CodePack* cp, WhileStmtNode* _node)
	{
		auto _begin_loc = cp->instructions.size();
		visit(cp, _node->condition);
		auto _condition_loc = cp->instructions.size();
		cp->instructions.push_back(Instruction(VM_CODE::IFNO, 0));
		visit(cp, _node->child);
		cp->instructions.push_back(Instruction(VM_CODE::JMP, -1 * (cp->instructions.size() - _begin_loc)));
		cp->instructions[_condition_loc] = Instruction(VM_CODE::IFNO, cp->instructions.size() - _condition_loc);
	}

	void CodeGen::visit(CodePack* cp, BreakStmtNode* _node)
	{
	}

	void CodeGen::visit(CodePack* cp, ReturnStmtNode* _node)
	{
		cp->instructions.push_back(Instruction(VM_CODE::RETURN, 0));
	}

	void CodeGen::visit(CodePack* cp, FuncDefNode* _node)
	{
		auto new_pack = vm->make_gcobject<CodePack>();
		auto new_func = vm->make_gcobject<Function>(new_pack, _node->parameters->identifiers.size());
		new_pack->prev = top_cp;
		top_cp = new_pack;

		visit(new_pack, _node->parameters);
		visit(new_pack, _node->block);
		new_pack->instructions.push_back(Instruction(VM_CODE::RETURN, 0));

		top_cp = new_pack->prev;

		new_func->codepack = new_pack;
		int _id = cp->constant.size();
		cp->constant.push_back(Object(new_func, Object::TYPE::CODE_PACK));
		cp->instructions.push_back(Instruction(VM_CODE::LOAD_C, _id));
		cp->instructions.push_back(Instruction(VM_CODE::CLOSURE, 0));

		if (_node->name)
		{
			int var_id = cp->findVarId(_node->name->name);
			if (var_id < 0)
			{
				var_id = cp->var_size++;
				cp->var_names.push_back(_node->name->name);
			}

			cp->instructions.push_back(Instruction(VM_CODE::STORE_V, var_id));
		}
	}

	void CodeGen::visit(CodePack* cp, FuncDefParamsNode* _node)
	{
		int index;
		for (auto i = _node->identifiers.begin();
			i != _node->identifiers.end(); ++i)
		{
			index = cp->var_size++;
			cp->var_names.push_back(*i);
		}
	}

	void CodeGen::visit(CodePack* cp, FuncCallNode* _node)
	{
		if (_node->params)
			visit(cp, _node->params);

		visit(cp, _node->exp);
		cp->instructions.push_back(Instruction(VM_CODE::CALL, 0));
	}

	void CodeGen::visit(CodePack* cp, FuncCallParamsNode* _node)
	{
		for (auto i = _node->children.begin(); i != _node->children.end(); ++i)
			visit(cp, *i);
	}

	void CodeGen::visit(CodePack* cp, PrintStmtNode* _node)
	{
		visit(cp, _node->expression);
		cp->instructions.push_back(Instruction(VM_CODE::OUT, 0));
	}

	void CodeGen::load()
	{
		vm->createEnvironment(global_cp);
	}

}
