#include "stdafx.h"
#include "codegen.h"
#include "svm_codes.h"
#include "string.h"
#include "context.h"

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
	CodeGen::VarType CodeGen::FindVar(const std::u16string& str)
	{
		for (auto i = state->variable_names.begin();
			i != state->variable_names.end(); ++i)
			if (*i == str)
				return VarType(VarType::LOCAL, i - state->variable_names.begin());

		for (auto i = state->upvalue_names.begin();
			i != state->upvalue_names.end(); ++i)
			if (*i == str)
				return VarType(VarType::UPVAL, i - state->upvalue_names.begin());

		if (state->prev != nullptr)
		{
			auto _p = FindVar(str);
			switch (_p.type())
			{
			case VarType::LOCAL:
			{
				state->required_upval_ids.push_back(_p.id());
				auto index = state->upvalue_names.size();
				state->upvalue_names.push_back(str);
				return VarType(VarType::UPVAL, index);
			}
			case VarType::UPVAL:
			{
				state->required_upval_ids.push_back(-1 - _p.id());
				auto index = state->upvalue_names.size();
				state->upvalue_names.push_back(str);
				return VarType(VarType::UPVAL, index);
			}

			}
		}
		return VarType(VarType::NONE);
	}

	CodeGen::CodeGen() : 
		parser(nullptr), final_function(nullptr),
		_var_statment(false), state(nullptr)
	{ 
		state = new GenState();
	}

	void CodeGen::AddInstruction(Instruction ins)
	{
		state->instructions.push_back(ins);
	}

	void CodeGen::generate(Parser* p)
	{
		parser = p;

		visit(parser->getRoot());

		AddInstruction(Instruction(VM_CODE::STOP, 0));
	}

	void CodeGen::PushState()
	{
		auto new_state = new GenState();
		new_state->prev = state;
		state = new_state;
	}

	void CodeGen::PopState()
	{
		auto _prev = state->prev;
		delete state;
		state = _prev;
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
				AddInstruction(Instruction(VM_CODE::OUT, 0));
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
			AddInstruction(Instruction(VM_CODE::PUSH_INT, -1));
			AddInstruction(Instruction(VM_CODE::MUL, 0));
			break;
		case OperatorType::NOT:
			AddInstruction(Instruction(VM_CODE::NOT, 0));
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
			AddInstruction(Instruction(VM_CODE::ADD, 0));
			break;
		case OperatorType::SUB:
			AddInstruction(Instruction(VM_CODE::SUB, 0));
			break;
		case OperatorType::MUL:
			AddInstruction(Instruction(VM_CODE::MUL, 0));
			break;
		case OperatorType::DIV:
			AddInstruction(Instruction(VM_CODE::DIV, 0));
			break;
		case OperatorType::MOD:
			AddInstruction(Instruction(VM_CODE::MOD, 0));
			break;
		case OperatorType::POW:
			AddInstruction(Instruction(VM_CODE::POW, 0));
			break;
		case OperatorType::GT:
			AddInstruction(Instruction(VM_CODE::GT, 0));
			break;
		case OperatorType::LT:
			AddInstruction(Instruction(VM_CODE::LT, 0));
			break;
		case OperatorType::GTEQ:
			AddInstruction(Instruction(VM_CODE::GTEQ, 0));
			break;
		case OperatorType::LTEQ:
			AddInstruction(Instruction(VM_CODE::LTEQ, 0));
			break;
		case OperatorType::EQ:
			AddInstruction(Instruction(VM_CODE::EQ, 0));
			break;
		default:
			throw std::runtime_error("unexpected operate type");
			AddInstruction(Instruction(VM_CODE::POP, 0));
		}
	}

	void CodeGen::visit(NumberNode* _node)
	{
		auto index = state->constant.size();

		if (_node->maybeInt)
			state->constant.push_back(
				Context::GetGc()->NewSmallInt(static_cast<TSmallInt>(_node->number)));
		else
			state->constant.push_back(
				Context::GetGc()->NewNumber(_node->number));

		AddInstruction(Instruction(VM_CODE::LOAD_C, index));
	}

	void CodeGen::visit(StringNode* _node)
	{
		auto index = state->constant.size();
		state->constant.push_back(static_cast<Object*>(String::FromU16String(_node->content)));
		AddInstruction(Instruction(VM_CODE::LOAD_C, index));
	}

	void CodeGen::visit(IdentifierNode* _node)
	{
		auto _var = FindVar(_node->name);

		switch (_var.type())
		{
		case VarType::GLOBAL:
			AddInstruction(Instruction(VM_CODE::LOAD_G, _var.id()));
			break;
		case VarType::LOCAL:
			AddInstruction(Instruction(VM_CODE::LOAD_V, _var.id()));
			break;
		case VarType::UPVAL:
			AddInstruction(Instruction(VM_CODE::LOAD_UPVAL, _var.id()));
			break;
		case VarType::NONE:
			throw std::runtime_error(std::string("variables not found"));
		}
	}

	void CodeGen::visit(AssignmentNode* _node)
	{
		// find if the var is exisits
		// if not exisits add a possition for it
		auto _id_node = dynamic_cast<IdentifierNode*>(_node->identifier);

		auto _var = FindVar(_id_node->name);

		int _id;
		switch(_var.type())
		{
		case VarType::GLOBAL:
			visit(_node->expression);
			AddInstruction(Instruction(VM_CODE::STORE_G, _var.id()));
			AddInstruction(Instruction(VM_CODE::LOAD_G, _var.id()));
			break;
		case VarType::LOCAL:
			visit(_node->expression);
			AddInstruction(Instruction(VM_CODE::STORE_V, _var.id()));
			AddInstruction(Instruction(VM_CODE::LOAD_V, _var.id()));
			break;
		case VarType::UPVAL:
			visit(_node->expression);
			AddInstruction(Instruction(VM_CODE::STORE_UPVAL, _var.id()));
			AddInstruction(Instruction(VM_CODE::LOAD_UPVAL, _var.id()));
			break;
			/*
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
			*/
		}

	}

	void CodeGen::visit(VarStmtNode* _node)
	{
		for (auto i = _node->children.begin(); i != _node->children.end(); ++i)
			visit(*i);
	}

	void CodeGen::visit(VarSubExprNode* _node)
	{

		auto _id_node = _node->varName;

		auto _id = state->variable_names.size();
		state->variable_names.push_back(_id_node->name);

		// you must add the name first and then visit the expression.
		// to generate the next code
		if (_node->expression)
		{
			visit(_node->expression);
			AddInstruction(Instruction(VM_CODE::STORE_V, _id));
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
		state->instructions.push_back(
			Instruction(VM_CODE::JMP, -1 * (state->instructions.size() - _begin_loc)));
		state->instructions[_condition_loc] = 
			Instruction(VM_CODE::IFNO, state->instructions.size() - _condition_loc);
	}

	void CodeGen::visit(BreakStmtNode* _node)
	{
	}

	void CodeGen::visit(ReturnStmtNode* _node)
	{
		if (_node->expression)
		{
			visit(_node->expression);
			AddInstruction(Instruction(VM_CODE::RETURN, 1));
		}
		else
			AddInstruction(Instruction(VM_CODE::RETURN, 0));
	}

	void CodeGen::visit(ClassDefNode* _node)
	{

	}

	void CodeGen::visit(FuncDefNode* _node)
	{
		int var_id = -1;
		if (_node->name)
		{
			var_id = state->variable_names.size();
			state->variable_names.push_back(_node->name->name);
		}

		auto new_func = Context::GetGc()->New<Function>();

		PushState();

		for (auto i = _node->parameters.begin();
			i != _node->parameters.end(); ++i)
			visit(*i);

		visit(_node->block);
		AddInstruction(Instruction(VM_CODE::RETURN, 0));

		new_func->cp = state->Finish();

		PopState();

		auto _id = state->constant.size();

		state->constant.push_back(new_func);

		AddInstruction(Instruction(VM_CODE::LOAD_C, _id));
		AddInstruction(Instruction(VM_CODE::CLOSURE, 0));

		if (var_id >= 0)
			AddInstruction(Instruction(VM_CODE::STORE_V, var_id));
	}

	void CodeGen::visit(FuncDefParamNode* _node)
	{
		state->variable_names.push_back(_node->name);
	}

	void CodeGen::visit(FuncCallNode* _node)
	{
		for (auto i = _node->parameters.begin(); 
			i != _node->parameters.end(); ++i)
			visit(*i);

		visit(_node->exp);
		AddInstruction(Instruction(VM_CODE::CALL, 0));
	}

	void CodeGen::visit(PrintStmtNode* _node)
	{
		visit(_node->expression);
		AddInstruction(Instruction(VM_CODE::OUT, 0));
	}

	CodeGen::~CodeGen()
	{
		delete state;
	}

	CodePack* CodeGen::GenState::Finish()
	{
		CodePack* cp = Context::GetGc()->New<CodePack>();

		cp->var_len = variable_names.size();
		cp->upval_len = upvalue_names.size();

		for (auto i = constant.begin();
			i != constant.end(); ++i)
			cp->constant->Push(*i);

		auto ins = Context::GetGc()->NewInstructionArray(instructions.size());
		for (unsigned int i = 0;
			i < instructions.size(); ++i)
			ins[i] = instructions[i];

		cp->instructions = ins;
		cp->instruction_length = instructions.size();

		auto arr = Context::GetGc()->NewArray<std::uint32_t>(required_upval_ids.size());
		for (unsigned int i = 0;
			i < required_upval_ids.size(); ++i)
			arr[i] = required_upval_ids[i];

		cp->required_upval_ids = arr;
		cp->required_upval_ids_len = required_upval_ids.size();

	}

}
