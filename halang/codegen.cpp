#include "stdafx.h"
#include "codegen.h"
#include "svm_codes.h"

namespace halang
{

	CodeGen::VarType CodeGen::findVar(CodePack * cp, IString _Str)
	{
		for (int i = 0; i < cp->var_names.size(); ++i)
			if (cp->var_names[i].first == _Str)
			{
				return VarType(VarType::LOCAL, i);
			}

		for (int i = 0; i < cp->upvalue_names.size(); ++i)
			if (cp->upvalue_names[i].first == _Str)
			{
				return VarType(VarType::UPVAL, i);
			}
		if (cp->prev)
		{
			VarType _p = findVar(cp->prev, _Str);
			switch (_p.type())
			{
			case VarType::LOCAL:
				cp->require_upvalues.push_back(_p.id());
				cp->upvalue_names.push_back(make_pair(IString(_Str), Type()));
				return VarType(VarType::UPVAL, cp->upvalue_size++);
				break;
			case VarType::UPVAL:
				cp->require_upvalues.push_back(-1 - _p.id());
				cp->upvalue_names.push_back(make_pair(IString(_Str), Type()));
				return VarType(VarType::UPVAL, cp->upvalue_size++);
				break;
			}
		}
		return VarType(VarType::NONE);
	}

	CodeGen::CodeGen(StackVM* _vm) : 
		vm(_vm), parser(nullptr)
	{ 
		top_cp = global_cp = vm->make_gcobject<CodePack>();
		state = new GenState();
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
		auto _var = findVar(cp, _node->name);

		switch (_var.type())
		{
		case VarType::GLOBAL:
			cp->instructions.push_back(Instruction(VM_CODE::LOAD_G, _var.id()));
			break;
		case VarType::LOCAL:
			cp->instructions.push_back(Instruction(VM_CODE::LOAD_V, _var.id()));
			break;
		case VarType::UPVAL:
			cp->instructions.push_back(Instruction(VM_CODE::LOAD_UPVAL, _var.id()));
			break;
		case VarType::NONE:
			ReportError(std::string("variables not found: ") + _node->name);
			break;
		}
	}

	void CodeGen::visit(CodePack* cp, AssignmentNode* _node)
	{
		// find if the var is exisits
		// if not exisits add a possition for it
		auto _id_node = dynamic_cast<IdentifierNode*>(_node->identifier);

		auto _var = findVar(cp, _id_node->name);

		int _id;
		switch(_var.type())
		{
		case VarType::GLOBAL:
			visit(cp, _node->expression);
			cp->instructions.push_back(Instruction(VM_CODE::STORE_G, _var.id()));
			cp->instructions.push_back(Instruction(VM_CODE::LOAD_G, _var.id()));
			break;
		case VarType::LOCAL:
			visit(cp, _node->expression);
			cp->instructions.push_back(Instruction(VM_CODE::STORE_V, _var.id()));
			cp->instructions.push_back(Instruction(VM_CODE::LOAD_V, _var.id()));
			break;
		case VarType::UPVAL:
			visit(cp, _node->expression);
			cp->instructions.push_back(Instruction(VM_CODE::STORE_UPVAL, _var.id()));
			cp->instructions.push_back(Instruction(VM_CODE::LOAD_UPVAL, _var.id()));
			break;
		case VarType::NONE:
			if (state->varStatement())
			{
				_id = cp->var_size++;
				cp->var_names.push_back(make_pair(IString(_id_node->name), Type()));

				// you must add the name first and then visit the expression.
				// to generate the next code
				visit(cp, _node->expression);
				cp->instructions.push_back(Instruction(VM_CODE::STORE_V, _id));
				cp->instructions.push_back(Instruction(VM_CODE::LOAD_V, _id));
			}
			else
			{
				// i don't know how to fix it, fuck you.
				ReportError(std::string("Identifier: ") + _id_node->name + " not found." );
			}
			break;
		}

	}

	void CodeGen::visit(CodePack* cp, VarStmtNode* _node)
	{
		state->setVarStatement(true);
		for (auto i = _node->children.begin(); i != _node->children.end(); ++i)
			visit(cp, *i);
		state->setVarStatement(false);
	}

	void CodeGen::visit(CodePack* cp, VarSubExprNode* _node)
	{

		auto _id_node = _node->varName;
		int _id = cp->var_size++;
		cp->var_names.push_back(make_pair(IString(_id_node->name), *_node->typeInfo));

		// you must add the name first and then visit the expression.
		// to generate the next code
		if (_node->expression)
		{
			visit(cp, _node->expression);
			cp->instructions.push_back(Instruction(VM_CODE::STORE_V, _id));
		}

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
		if (_node->expression)
		{
			visit(cp, _node->expression);
			cp->instructions.push_back(Instruction(VM_CODE::RETURN, 1));
		}
		else
			cp->instructions.push_back(Instruction(VM_CODE::RETURN, 0));
	}

	void CodeGen::visit(CodePack* cp, FuncDefNode* _node)
	{
		int var_id = -1;
		if (_node->name)
		{
			var_id = cp->var_size++;
			cp->var_names.push_back(make_pair(IString(_node->name->name), Type()));
		}
		auto new_pack = vm->make_gcobject<CodePack>();
		auto new_func = vm->make_gcobject<Function>(new_pack, _node->parameters.size());
		new_pack->prev = top_cp;
		top_cp = new_pack;

		for (auto i = _node->parameters.begin();
			i != _node->parameters.end(); ++i)
			visit(new_pack, *i);

		visit(new_pack, _node->block);
		new_pack->instructions.push_back(Instruction(VM_CODE::RETURN, 0));

		top_cp = new_pack->prev;

		new_func->codepack = new_pack;
		int _id = cp->constant.size();
		cp->constant.push_back(Object(new_func, Object::TYPE::CODE_PACK));
		cp->instructions.push_back(Instruction(VM_CODE::LOAD_C, _id));
		cp->instructions.push_back(Instruction(VM_CODE::CLOSURE, 0));

		if (var_id >= 0)
			cp->instructions.push_back(Instruction(VM_CODE::STORE_V, var_id));
	}

	void CodeGen::visit(CodePack* cp, FuncDefParamNode* _node)
	{
		cp->var_names.push_back(make_pair(IString(_node->name), *_node->typeInfo));
	}

	void CodeGen::visit(CodePack* cp, FuncCallNode* _node)
	{
		for (auto i = _node->parameters.begin(); 
			i != _node->parameters.end(); ++i)
			visit(cp, *i);

		visit(cp, _node->exp);
		cp->instructions.push_back(Instruction(VM_CODE::CALL, 0));
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

	CodeGen::~CodeGen()
	{
		delete state;
	}

}
