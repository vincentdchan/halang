#include "stdafx.h"
#include "codegen.h"
#include "svm_codes.h"

namespace halang
{

	/// <summary>
	/// Find the variable name in the codepack
	/// if it isn't exist, find it in the upvalue table
	///
	/// Find the variable in the previous codepack.
	/// if the variable name in the previous codepack and
	///		if it's a local in the previous codepack
	///			let it be a upvalue
	///		else if it's already a upvalue
	///			you guess what
	/// </summary>
	CodeGen::VarType CodeGen::findVar(CodePack * cp, IString _Str)
	{
		for (int i = 0; i < cp->var_names.size(); ++i)
			if (cp->var_names[i] == _Str)
				return VarType(VarType::LOCAL, i);

		for (int i = 0; i < cp->upvalue_names.size(); ++i)
			if (cp->upvalue_names[i] == _Str)
				return VarType(VarType::UPVAL, i);

		if (cp->prev)
		{
			VarType _p = findVar(cp->prev, _Str);
			switch (_p.type())
			{
			case VarType::LOCAL:
			{
				cp->require_upvalues.push_back(_p.id());
				auto t = cp->addUpValue(IString(_Str));
				return VarType(VarType::UPVAL, t);
			}
			case VarType::UPVAL:
			{
				cp->require_upvalues.push_back(-1 - _p.id());
				auto i = cp->addUpValue(IString(_Str));
				return VarType(VarType::UPVAL, i);
			}

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

		visit(parser->getRoot());
		global_cp->instructions.push_back(Instruction(VM_CODE::STOP, 0));
	}

	void CodeGen::visit(Node* _node)
	{
		_node->visit(this);
	}

	void CodeGen::visit(BlockExprNode* _node)
	{
		auto cp = top_cp;
		for (auto i = _node->children.begin(); i != _node->children.end(); ++i)
		{
			(*i)->visit(this);

			// TODO: debug
			if ((*i)->asUnaryExpression() || 
				(*i)->asBinaryExpression() || 
				(*i)->asIdentifier() || 
				(*i)->asNumber())
				cp->instructions.push_back(Instruction(VM_CODE::OUT, 0));
		}
	}

	void CodeGen::visit(ListExprNode* _node)
	{

	}

	void CodeGen::visit(InvokeExprNode* _node)
	{
	}

	void CodeGen::visit(UnaryExprNode* _node)
	{
		auto cp = top_cp;

		visit(_node->child);
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

	void CodeGen::visit(BinaryExprNode* _node)
	{
		auto cp = top_cp;
		visit(_node->left);
		visit(_node->right);
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

	void CodeGen::visit(NumberNode* _node)
	{
		auto cp = top_cp;
		auto index = cp->constant.size();

		if (_node->maybeInt)
			cp->constant.push_back(Object(static_cast<int>(_node->number)));
		cp->constant.push_back(Object(_node->number));

		cp->instructions.push_back(Instruction(VM_CODE::LOAD_C, index));
	}

	void CodeGen::visit(StringNode* _node)
	{
		auto cp = top_cp;
		auto index = cp->constant.size();
		cp->constant.push_back(Object(_node->content));

		cp->instructions.push_back(Instruction(VM_CODE::LOAD_C, index));
	}

	void CodeGen::visit(IdentifierNode* _node)
	{
		auto cp = top_cp;
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

	void CodeGen::visit(AssignmentNode* _node)
	{
		auto cp = top_cp;
		// find if the var is exisits
		// if not exisits add a possition for it
		auto _id_node = dynamic_cast<IdentifierNode*>(_node->identifier);

		auto _var = findVar(cp, _id_node->name);

		int _id;
		switch(_var.type())
		{
		case VarType::GLOBAL:
			visit(_node->expression);
			cp->instructions.push_back(Instruction(VM_CODE::STORE_G, _var.id()));
			cp->instructions.push_back(Instruction(VM_CODE::LOAD_G, _var.id()));
			break;
		case VarType::LOCAL:
			visit(_node->expression);
			cp->instructions.push_back(Instruction(VM_CODE::STORE_V, _var.id()));
			cp->instructions.push_back(Instruction(VM_CODE::LOAD_V, _var.id()));
			break;
		case VarType::UPVAL:
			visit(_node->expression);
			cp->instructions.push_back(Instruction(VM_CODE::STORE_UPVAL, _var.id()));
			cp->instructions.push_back(Instruction(VM_CODE::LOAD_UPVAL, _var.id()));
			break;
		case VarType::NONE:
			if (state->varStatement())
			{
				_id = cp->addVar(IString(_id_node->name));

				// you must add the name first and then visit the expression.
				// to generate the next code
				visit(_node->expression);
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

	void CodeGen::visit(VarStmtNode* _node)
	{
		auto cp = top_cp;
		state->setVarStatement(true);
		for (auto i = _node->children.begin(); i != _node->children.end(); ++i)
			visit(*i);
		state->setVarStatement(false);
	}

	void CodeGen::visit(VarSubExprNode* _node)
	{
		auto cp = top_cp;
		auto _id_node = _node->varName;
		int _id = cp->addVar(IString(_id_node->name));

		// you must add the name first and then visit the expression.
		// to generate the next code
		if (_node->expression)
		{
			visit(_node->expression);
			cp->instructions.push_back(Instruction(VM_CODE::STORE_V, _id));
		}

	}

	void CodeGen::visit(IfStmtNode* _node)
	{
		auto cp = top_cp;
		int jmp_val;
		visit(_node->condition);
		auto jmp_loc = cp->instructions.size();
		cp->instructions.push_back(Instruction(VM_CODE::IFNO, 1));
		visit(_node->true_branch);
		auto true_finish_loc = cp->instructions.size();
		cp->instructions.push_back(Instruction(VM_CODE::JMP, 1));
		// if condition not ture, jmp to the right location
		jmp_val = cp->instructions.size() - jmp_loc;
		cp->instructions[jmp_loc] = Instruction(VM_CODE::IFNO, jmp_val); 
		if (_node->false_branch)
		{
			visit(_node->false_branch);
			jmp_val = cp->instructions.size() - true_finish_loc;
			cp->instructions[true_finish_loc] = Instruction(VM_CODE::JMP, jmp_val);
		}
	}

	void CodeGen::visit(WhileStmtNode* _node)
	{
		auto cp = top_cp;
		auto _begin_loc = cp->instructions.size();
		visit(_node->condition);
		auto _condition_loc = cp->instructions.size();
		cp->instructions.push_back(Instruction(VM_CODE::IFNO, 0));
		visit(_node->child);
		cp->instructions.push_back(Instruction(VM_CODE::JMP, -1 * (cp->instructions.size() - _begin_loc)));
		cp->instructions[_condition_loc] = Instruction(VM_CODE::IFNO, cp->instructions.size() - _condition_loc);
	}

	void CodeGen::visit(BreakStmtNode* _node)
	{
	}

	void CodeGen::visit(ReturnStmtNode* _node)
	{
		auto cp = top_cp;
		if (_node->expression)
		{
			visit(_node->expression);
			cp->instructions.push_back(Instruction(VM_CODE::RETURN, 1));
		}
		else
			cp->instructions.push_back(Instruction(VM_CODE::RETURN, 0));
	}

	void CodeGen::visit(ClassDefNode* _node)
	{

	}

	void CodeGen::visit(FuncDefNode* _node)
	{
		auto cp = top_cp;
		int var_id = -1;
		if (_node->name)
			var_id = cp->addVar(IString(_node->name->name));
		auto new_pack = vm->make_gcobject<CodePack>();
		auto new_func = vm->make_gcobject<Function>(new_pack, _node->parameters.size());

		// Push
		new_pack->prev = top_cp;
		top_cp = new_pack;

		for (auto i = _node->parameters.begin();
			i != _node->parameters.end(); ++i)
			visit(*i);

		visit(_node->block);
		new_pack->instructions.push_back(Instruction(VM_CODE::RETURN, 0));

		// Pop
		top_cp = new_pack->prev;

		new_func->codepack = new_pack;
		int _id = cp->constant.size();
		cp->constant.push_back(Object(new_func, Object::TYPE::CODE_PACK));
		cp->instructions.push_back(Instruction(VM_CODE::LOAD_C, _id));
		cp->instructions.push_back(Instruction(VM_CODE::CLOSURE, 0));

		if (var_id >= 0)
			cp->instructions.push_back(Instruction(VM_CODE::STORE_V, var_id));
	}

	void CodeGen::visit(FuncDefParamNode* _node)
	{
		top_cp->addVar(IString(_node->name));
	}

	void CodeGen::visit(FuncCallNode* _node)
	{
		auto cp = top_cp;
		for (auto i = _node->parameters.begin(); 
			i != _node->parameters.end(); ++i)
			visit(*i);

		visit(_node->exp);
		cp->instructions.push_back(Instruction(VM_CODE::CALL, 0));
	}

	void CodeGen::visit(PrintStmtNode* _node)
	{
		visit(_node->expression);
		top_cp->instructions.push_back(Instruction(VM_CODE::OUT, 0));
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
