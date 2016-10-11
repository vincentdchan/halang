#include "stdafx.h"
#include "codegen.h"
#include "svm_codes.h"
#include "String.h"
#include "context.h"
#include <string>
#include <codecvt>
#include <locale>

namespace halang
{

	CodePack* CodeGen::GenState::GenerateCodePack(GenState* gs)
	{
		auto cp = Context::GetGC()->New<CodePack>();

		cp->_require_upvales_size = gs->require_upvalues.size();
		std::copy(gs->require_upvalues.begin(), gs->require_upvalues.end(), cp->_require_upvalues);

		cp->_const_size = gs->constant.size();
		cp->_constants = new Value[cp->_const_size];
		std::copy(gs->constant.begin(), gs->constant.end(), cp->_constants);

		cp->_var_names_size = gs->var_names.size();
		cp->_var_names = new String*[cp->_var_names_size];
		for (unsigned int i = 0;
			i < cp->_var_names_size; ++i)
		{
			cp->_var_names[i] = String::FromU16String(gs->var_names[i]);
		}

		cp->_upval_names_size = gs->upvalue_names.size();
		cp->_upval_names = new String*[cp->_upval_names_size];
		for (unsigned int i = 0;
			i < cp->_upval_names_size; ++i)
		{
			cp->_upval_names[i] = String::FromU16String(gs->upvalue_names[i]);
		}

		cp->_upval_names_size = gs->upvalue_names.size();

		return cp;

	}

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
	CodeGen::VarType CodeGen::FindVar(GenState * cs, const std::u16string& _Str)
	{
		for (int i = 0; i < cs->var_names.size(); ++i)
			if (cs->var_names[i] == _Str)
				return VarType(VarType::TYPE::LOCAL, i);

		for (int i = 0; i < cs->upvalue_names.size(); ++i)
			if (cs->upvalue_names[i] == _Str)
				return VarType(VarType::TYPE::UPVAL, i);

		if (cs->prev)
		{
			VarType _p = FindVar(cs->prev, _Str);
			switch (_p.type())
			{
			case VarType::TYPE::LOCAL:
			{
				cs->require_upvalues.push_back(_p.id());
				auto t = cs->AddUpValue(_Str);
				return VarType(VarType::TYPE::UPVAL, t);
			}
			case VarType::TYPE::UPVAL:
			{
				cs->require_upvalues.push_back(-1 - _p.id());
				auto i = cs->AddUpValue(_Str);
				return VarType(VarType::TYPE::UPVAL, i);
			}

			}
		}
		return VarType(VarType::TYPE::NONE);
	}

	CodeGen::CodeGen(StackVM* _vm) : 
		vm(_vm), parser(nullptr), name(nullptr)
	{ 
		auto new_state = new GenState();
		new_state->prev = state;
		state = new_state;
	}

	void CodeGen::AddInst(Instruction inst)
	{
		state->instructions.push_back(inst);
	}

	void CodeGen::generate(Parser* p)
	{
		parser = p;

		visit(parser->getRoot());
		AddInst(Instruction(VM_CODE::STOP, 0));
	}

	void CodeGen::visit(Node* _node)
	{
		_node->visit(this);
	}

	void CodeGen::visit(BlockExprNode* _node)
	{
		for (auto i = _node->children.begin(); i != _node->children.end(); ++i)
		{
			(*i)->visit(this);

			// TODO: debug
			if ((*i)->asUnaryExpression() ||
				(*i)->asBinaryExpression() ||
				(*i)->asIdentifier() ||
				(*i)->asNumber())
				AddInst(Instruction(VM_CODE::OUT, 0));
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
		visit(_node->child);
		switch (_node->op)
		{
		case OperatorType::SUB:
			AddInst(Instruction(VM_CODE::PUSH_INT, -1));
			AddInst(Instruction(VM_CODE::MUL, 0));
			break;
		case OperatorType::NOT:
			AddInst(Instruction(VM_CODE::NOT, 0));
			break;
		}
	}

	void CodeGen::visit(BinaryExprNode* _node)
	{
		visit(_node->left);
		visit(_node->right);
		switch (_node->op)
		{
		case OperatorType::ADD:
			AddInst(Instruction(VM_CODE::ADD, 0));
			break;
		case OperatorType::SUB:
			AddInst(Instruction(VM_CODE::SUB, 0));
			break;
		case OperatorType::MUL:
			AddInst(Instruction(VM_CODE::MUL, 0));
			break;
		case OperatorType::DIV:
			AddInst(Instruction(VM_CODE::DIV, 0));
			break;
		case OperatorType::MOD:
			AddInst(Instruction(VM_CODE::MOD, 0));
			break;
		case OperatorType::POW:
			AddInst(Instruction(VM_CODE::POW, 0));
			break;
		case OperatorType::GT:
			AddInst(Instruction(VM_CODE::GT, 0));
			break;
		case OperatorType::LT:
			AddInst(Instruction(VM_CODE::LT, 0));
			break;
		case OperatorType::GTEQ:
			AddInst(Instruction(VM_CODE::GTEQ, 0));
			break;
		case OperatorType::LTEQ:
			AddInst(Instruction(VM_CODE::LTEQ, 0));
			break;
		case OperatorType::EQ:
			AddInst(Instruction(VM_CODE::EQ, 0));
			break;
		default:
			// runtime error
			AddInst(Instruction(VM_CODE::POP, 0));
			// pack.instructions.push_back(Instruction(VM_CODE::POP, 0));
		}
	}

	void CodeGen::visit(NumberNode* _node)
	{
		auto index = state->constant.size();

		if (_node->maybeInt)
			state->constant.push_back(Value(static_cast<TSmallInt>(_node->number)));
		state->constant.push_back(Value(_node->number));

		AddInst(Instruction(VM_CODE::LOAD_C, index));
	}

	void CodeGen::visit(StringNode* _node)
	{
		auto index = state->constant.size();
		state->constant.push_back(Value(String::FromU16String(_node->content)));

		AddInst(Instruction(VM_CODE::LOAD_C, index));
	}

	void CodeGen::visit(IdentifierNode* _node)
	{
		auto _var = FindVar(state, _node->name);

		switch (_var.type())
		{
		case VarType::TYPE::GLOBAL:
			AddInst(Instruction(VM_CODE::LOAD_G, _var.id()));
			break;
		case VarType::TYPE::LOCAL:
			AddInst(Instruction(VM_CODE::LOAD_V, _var.id()));
			break;
		case VarType::TYPE::UPVAL:
			AddInst(Instruction(VM_CODE::LOAD_UPVAL, _var.id()));
			break;
		case VarType::TYPE::NONE:
			ReportError(std::string("variables not found"));
			break;
		}
	}

	void CodeGen::visit(AssignmentNode* _node)
	{
		// find if the var is exisits
		// if not exisits add a possition for it
		auto _id_node = dynamic_cast<IdentifierNode*>(_node->identifier);

		auto _var = FindVar(state, _id_node->name);

		int _id;
		switch(_var.type())
		{
		case VarType::TYPE::GLOBAL:
			visit(_node->expression);
			AddInst(Instruction(VM_CODE::STORE_G, _var.id()));
			AddInst(Instruction(VM_CODE::LOAD_G, _var.id()));
			break;
		case VarType::TYPE::LOCAL:
			visit(_node->expression);
			AddInst(Instruction(VM_CODE::STORE_V, _var.id()));
			AddInst(Instruction(VM_CODE::LOAD_V, _var.id()));
			break;
		case VarType::TYPE::UPVAL:
			visit(_node->expression);
			AddInst(Instruction(VM_CODE::STORE_UPVAL, _var.id()));
			AddInst(Instruction(VM_CODE::LOAD_UPVAL, _var.id()));
			break;
		case VarType::TYPE::NONE:
			if (_var_statement)
			{
				_id = state->AddVariable(_id_node->name);

				// you must add the name first and then visit the expression.
				// to generate the next code
				visit(_node->expression);
				AddInst(Instruction(VM_CODE::STORE_V, _id));
				AddInst(Instruction(VM_CODE::LOAD_V, _id));
			}
			else
			{
				// i don't know how to fix it, fuck you.
				ReportError(std::string("Identifier: not found."));
			}
			break;
		}

	}

	void CodeGen::visit(VarStmtNode* _node)
	{
		_var_statement = true;
		for (auto i = _node->children.begin(); i != _node->children.end(); ++i)
			visit(*i);
		_var_statement = false;
	}

	void CodeGen::visit(VarSubExprNode* _node)
	{
		auto _id_node = _node->varName;
		int _id = state->AddVariable(_id_node->name);

		// you must add the name first and then visit the expression.
		// to generate the next code
		if (_node->expression)
		{
			visit(_node->expression);
			AddInst(Instruction(VM_CODE::STORE_V, _id));
		}

	}

	void CodeGen::visit(IfStmtNode* _node)
	{
		int jmp_val;
		visit(_node->condition);
		auto jmp_loc = state->instructions.size();
		state->instructions.push_back(Instruction(VM_CODE::IFNO, 1));
		visit(_node->true_branch);
		auto true_finish_loc = state->instructions.size();
		state->instructions.push_back(Instruction(VM_CODE::JMP, 1));
		// if condition not ture, jmp to the right location
		jmp_val = state->instructions.size() - jmp_loc;
		state->instructions[jmp_loc] = Instruction(VM_CODE::IFNO, jmp_val); 
		if (_node->false_branch)
		{
			visit(_node->false_branch);
			jmp_val = state->instructions.size() - true_finish_loc;
			state->instructions[true_finish_loc] = Instruction(VM_CODE::JMP, jmp_val);
		}
	}

	void CodeGen::visit(WhileStmtNode* _node)
	{
		auto _begin_loc = state->instructions.size();
		visit(_node->condition);
		auto _condition_loc = state->instructions.size();
		state->instructions.push_back(Instruction(VM_CODE::IFNO, 0));
		visit(_node->child);
		state->instructions.push_back(Instruction(VM_CODE::JMP, -1 * (state->instructions.size() - _begin_loc)));
		state->instructions[_condition_loc] = Instruction(VM_CODE::IFNO, state->instructions.size() - _condition_loc);
	}

	void CodeGen::visit(BreakStmtNode* _node)
	{
	}

	void CodeGen::visit(ReturnStmtNode* _node)
	{
		if (_node->expression)
		{
			visit(_node->expression);
			AddInst(Instruction(VM_CODE::RETURN, 1));
		}
		else
			AddInst(Instruction(VM_CODE::RETURN, 0));
	}

	void CodeGen::visit(ClassDefNode* _node)
	{

	}

	void CodeGen::visit(FuncDefNode* _node)
	{
		auto c_state = state;
		int var_id = -1;
		if (_node->name)
			var_id = state->AddVariable(_node->name->name);

		auto new_state = new GenState();
		auto new_fun = Context::GetGC()->New<Function>();

		new_state->prev = state;
		state = new_state;

		for (auto i = _node->parameters.begin();
			i != _node->parameters.end(); ++i)
			visit(*i);

		visit(_node->block);
		AddInst(Instruction(VM_CODE::RETURN, 0));

		state = new_state->prev;
		new_fun->codepack = GenState::GenerateCodePack(new_state);
		new_fun->paramsSize = _node->parameters.size();

		delete new_state;

		int const_id = state->constant.size();
		state->constant.push_back(new_fun->toValue());
		AddInst(Instruction(VM_CODE::LOAD_C, const_id));
		AddInst(Instruction(VM_CODE::CLOSURE, 0));

		if (var_id >= 0)
			AddInst(Instruction(VM_CODE::STORE_V, var_id));
	}

	void CodeGen::visit(FuncDefParamNode* _node)
	{
		state->AddVariable(_node->name);
	}

	void CodeGen::visit(FuncCallNode* _node)
	{
		for (auto i = _node->parameters.begin(); 
			i != _node->parameters.end(); ++i)
			visit(*i);

		visit(_node->exp);
		AddInst(Instruction(VM_CODE::CALL, 0));
	}

	void CodeGen::visit(PrintStmtNode* _node)
	{
		visit(_node->expression);
		AddInst(Instruction(VM_CODE::OUT, 0));
	}

	void CodeGen::load()
	{
	}

	CodeGen::~CodeGen()
	{
		if (name != nullptr)
			delete name;
		delete state;
	}

}
