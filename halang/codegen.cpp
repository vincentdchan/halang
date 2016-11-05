#include "stdafx.h"
#include "codegen.h"
#include "svm_codes.h"
#include "String.h"
#include "context.h"
#include <string>
#include "util.h"

#define TEXT(T) String::FromCharArray(T)->toValue()

namespace halang
{

	CodeGen::GenState* CodeGen::GenState::CreateEqualState(GenState * state)
	{
		auto result = new GenState();
		result->isNew = false;
		result->prev = state;
		result->father_state = state->father_state;
		result->upvalue_names = state->upvalue_names;
		result->require_upvalues = state->require_upvalues;

		result->copyEntry();
		result->_var_names_size = state->_var_names_size;
		result->_max_entries_size = state->_max_entries_size;

		result->constant = state->constant;
		result->instructions = state->instructions;
		return result;
	}

	CodeGen::GenState* CodeGen::GenState::CreateNewState(GenState * state)
	{
		auto result = new GenState();
		result->isNew = true;
		result->prev = state;
		if (state != nullptr)
			result->_level = state->_level + 1;
		else
			result->_level = 0;

		result->father_state = state;
		result->upvalue_names = new std::vector<std::u16string>();
		result->require_upvalues = new std::vector<int>();
		result->_max_entries_size = new unsigned int(0);
		result->_var_names_size = 0;

		result->constant = new std::vector<Value>();
		result->instructions = new std::vector<Instruction>();
		return result;
	}

	bool CodeGen::GenState::ExistName(const std::u16string& _name) const
	{
		auto _hash = std::hash<std::u16string>{}(_name);
		auto _index = _hash % ENTRY_SIZE;

		auto ptr = var_names_entries[_index];
		while (ptr != nullptr) {
			if (ptr->hash == _hash) {
				return true;
			}
			ptr = ptr->next;
		}
		return false;
	}

	bool CodeGen::GenState::TryGetVarId(const std::u16string& _name, int &_id) const
	{
		auto _hash = std::hash<std::u16string>{}(_name);
		auto _index = _hash % ENTRY_SIZE;

		auto ptr = var_names_entries[_index];
		while (ptr != nullptr) {
			if (ptr->hash == _hash) {
				_id = ptr->givenId;
				return true;
			}
			ptr = ptr->next;
		}
		return false;
	}

	CodeGen::GenState::size_type 
		CodeGen::GenState::AddVariable(const std::u16string& name)
	{
		auto new_entry = make_shared<VariableEntry>(name, *(_max_entries_size));
		auto _index = new_entry->hash % ENTRY_SIZE;
		new_entry->next = var_names_entries[_index];
		var_names_entries[_index] = new_entry;
		_var_names_size++;
		return (*_max_entries_size)++;
	}

	CodeGen::GenState::size_type
		CodeGen::GenState::size()
	{
		return _var_names_size;
	}

	CodeGen::GenState::size_type 
		CodeGen::GenState::AddUpValue(const std::u16string& name)
	{
		size_type i = upvalue_names->size();
		upvalue_names->push_back(name);
		return i;
	}

	void CodeGen::GenState::forEachVarNames(std::function<void (const std::u16string&, int)> _fun) {
		for (auto i = var_names_entries.begin();
			i != var_names_entries.end(); i++) {
			auto ptr = *i;
			while (ptr != nullptr)
			{
				_fun(ptr->name, ptr->givenId);
				ptr = ptr->next;
			}
		}
	}

	CodeGen::GenState::~GenState()
	{
		if (isNew)
		{
			delete upvalue_names;
			delete require_upvalues;
			delete _max_entries_size;
			delete constant;
			delete instructions;
		}
	}

	CodePack* CodeGen::GenState::GenerateCodePack(GenState* gs)
	{
		if (!gs->isNew)
			throw std::logic_error("You can only generate new code pack");

		auto cp = Context::GetGC()->New<CodePack>();

		// copy instructions
		auto needed_size = gs->GetInstructionVector()->size();
		cp->_instructions = new Instruction[needed_size];
		cp->_instructions_size = 0;
		for (auto i = gs->GetInstructionVector()->begin();
			i != gs->GetInstructionVector()->end(); ++i)
			cp->_instructions[cp->_instructions_size++] = *i;

		// copy require_upvalues;
		needed_size = gs->GetRequireUpvaluesVector()->size();
		if (needed_size > 0)
		{
			cp->_require_upvalues = new int[needed_size];
			cp->_require_upvalues_size = 0;
			for (auto i = gs->GetRequireUpvaluesVector()->begin();
				i != gs->GetRequireUpvaluesVector()->end(); ++i)
				cp->_require_upvalues[cp->_require_upvalues_size++] = *i;
		}

		// copy constants
		needed_size = gs->GetConstantVector()->size();
		cp->_const_size = 0;
		cp->_constants = new Value[needed_size]();
		for (auto i = gs->GetConstantVector()->begin();
			i != gs->GetConstantVector()->end(); ++i)
			cp->_constants[cp->_const_size++] = *i;

		// copy varnames;
		cp->GenerateVarNamesArray(gs->MaxVarNamesSize());
		gs->forEachVarNames([&cp](const std::u16string& _name, int id) 
		{
			cp->SetVarName(id, String::FromU16String(_name));
		});

		// copy upval names;
		cp->GenerateUpValNamesArray(gs->GetUpValuesVector()->size());
		for (unsigned int i = 0;
			i < cp->_upval_names_size; ++i)
		{
			cp->SetUpValName(i, 
				String::FromU16String(gs->GetUpValuesVector()->at(i)));
		}

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
		int _var_id;
		if (cs->TryGetVarId(_Str, _var_id))
			return VarType(VarType::TYPE::LOCAL, _var_id);

		for (int i = 0; i < cs->GetUpValuesVector()->size(); ++i)
			if (cs->GetUpValuesVector()->at(i) == _Str)
				return VarType(VarType::TYPE::UPVAL, i);

		if (cs->GetFather())
		{
			VarType _p = FindVar(cs->GetFather(), _Str);
			switch (_p.type())
			{
			case VarType::TYPE::LOCAL:
			{
				cs->GetRequireUpvaluesVector()->push_back(_p.id());
				auto t = cs->AddUpValue(_Str);
				return VarType(VarType::TYPE::UPVAL, t);
			}
			case VarType::TYPE::UPVAL:
			{
				cs->GetRequireUpvaluesVector()->push_back(-1 - _p.id());
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
		auto new_state = GenerateDefaultState();
		state = new_state;

		_while_statement = false;
	}

	CodeGen::GenState* CodeGen::GenerateDefaultState()
	{
		auto state = GenState::CreateNewState();
		auto _print_fun_ = Context::GetGC()->New<Function>(Context::_print_);
		auto _fun_id = state->AddConstant(_print_fun_->toValue());

		auto var_id = state->AddVariable(u"print");
		state->AddInstruction(VM_CODE::LOAD_C, _fun_id);
		state->AddInstruction(VM_CODE::STORE_V, var_id);
		return state;
	}

	void CodeGen::AddInst(Instruction inst)
	{
		state->AddInstruction(inst);
	}

	Function* CodeGen::generate(Parser* p)
	{
		parser = p;

		visit(parser->getRoot());
		AddInst(Instruction(VM_CODE::STOP, 0));

		return Context::GetGC()->New<Function>(GenState::GenerateCodePack(state));
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
		visit(_node->source);
		auto _name_id = state->AddConstant(
			String::FromU16String(_node->id->name)->toValue());
		AddInst(Instruction(VM_CODE::LOAD_C, _name_id));
		AddInst(Instruction(VM_CODE::DOT, 0));
	}

	void CodeGen::visit(UnaryExprNode* _node)
	{
		visit(_node->child);
		unsigned int id;
		switch (_node->op)
		{
		case OperatorType::SUB:
			id = state->AddConstant(Context::StringBuffer::__REVERSE__->toValue());
			AddInst(Instruction(VM_CODE::LOAD_C, id));
			AddInst(Instruction(VM_CODE::DOT, 0));
			AddInst(Instruction(VM_CODE::CALL, 0));
			break;
		case OperatorType::NOT:
			id = state->AddConstant(Context::StringBuffer::__NOT__->toValue());
			AddInst(Instruction(VM_CODE::LOAD_C, id));
			AddInst(Instruction(VM_CODE::DOT, 0));
			AddInst(Instruction(VM_CODE::CALL, 0));
			break;
		}
	}

	void CodeGen::visit(BinaryExprNode* _node)
	{
		visit(_node->right);
		visit(_node->left);

		unsigned int id;
		switch (_node->op)
		{
		case OperatorType::ADD:
			id = state->AddConstant(Context::StringBuffer::__ADD__->toValue());
			AddInst(Instruction(VM_CODE::LOAD_C, id));
			AddInst(Instruction(VM_CODE::DOT, 0));
			AddInst(Instruction(VM_CODE::CALL, 1));
			break;
		case OperatorType::SUB:
			id = state->AddConstant(Context::StringBuffer::__SUB__->toValue());
			AddInst(Instruction(VM_CODE::LOAD_C, id));
			AddInst(Instruction(VM_CODE::DOT, 0));
			AddInst(Instruction(VM_CODE::CALL, 1));
			break;
		case OperatorType::MUL:
			id = state->AddConstant(Context::StringBuffer::__MUL__->toValue());
			AddInst(Instruction(VM_CODE::LOAD_C, id));
			AddInst(Instruction(VM_CODE::DOT, 0));
			AddInst(Instruction(VM_CODE::CALL, 1));
			break;
		case OperatorType::DIV:
			id = state->AddConstant(Context::StringBuffer::__DIV__->toValue());
			AddInst(Instruction(VM_CODE::LOAD_C, id));
			AddInst(Instruction(VM_CODE::DOT, 0));
			AddInst(Instruction(VM_CODE::CALL, 1));
			break;
		case OperatorType::MOD:
			id = state->AddConstant(Context::StringBuffer::__MOD__->toValue());
			AddInst(Instruction(VM_CODE::LOAD_C, id));
			AddInst(Instruction(VM_CODE::DOT, 0));
			AddInst(Instruction(VM_CODE::CALL, 1));
			break;
		case OperatorType::POW:
			id = state->AddConstant(TEXT("__pow__"));
			AddInst(Instruction(VM_CODE::LOAD_C, id));
			AddInst(Instruction(VM_CODE::DOT, 0));
			AddInst(Instruction(VM_CODE::CALL, 1));
			break;
		case OperatorType::GT:
			id = state->AddConstant(Context::StringBuffer::__GT__->toValue());
			AddInst(Instruction(VM_CODE::LOAD_C, id));
			AddInst(Instruction(VM_CODE::DOT, 0));
			AddInst(Instruction(VM_CODE::CALL, 1));
			break;
		case OperatorType::LT:
			id = state->AddConstant(Context::StringBuffer::__LT__->toValue());
			AddInst(Instruction(VM_CODE::LOAD_C, id));
			AddInst(Instruction(VM_CODE::DOT, 0));
			AddInst(Instruction(VM_CODE::CALL, 1));
			break;
		case OperatorType::GTEQ:
			id = state->AddConstant(Context::StringBuffer::__GTEQ__->toValue());
			AddInst(Instruction(VM_CODE::LOAD_C, id));
			AddInst(Instruction(VM_CODE::DOT, 0));
			AddInst(Instruction(VM_CODE::CALL, 1));
			break;
		case OperatorType::LTEQ:
			id = state->AddConstant(Context::StringBuffer::__LTEQ__->toValue());
			AddInst(Instruction(VM_CODE::LOAD_C, id));
			AddInst(Instruction(VM_CODE::DOT, 0));
			AddInst(Instruction(VM_CODE::CALL, 1));
			break;
		case OperatorType::EQ:
			id = state->AddConstant(Context::StringBuffer::__EQ__->toValue());
			AddInst(Instruction(VM_CODE::LOAD_C, id));
			AddInst(Instruction(VM_CODE::DOT, 0));
			AddInst(Instruction(VM_CODE::CALL, 1));
			break;
		case OperatorType::AND:
			id = state->AddConstant(Context::StringBuffer::__AND__->toValue());
			AddInst(Instruction(VM_CODE::LOAD_C, id));
			AddInst(Instruction(VM_CODE::DOT, 0));
			AddInst(Instruction(VM_CODE::CALL, 1));
			break;
		case OperatorType::OR:
			id = state->AddConstant(Context::StringBuffer::__OR__->toValue());
			AddInst(Instruction(VM_CODE::LOAD_C, id));
			AddInst(Instruction(VM_CODE::DOT, 0));
			AddInst(Instruction(VM_CODE::CALL, 1));
			break;
		default:
			// runtime error
			AddInst(Instruction(VM_CODE::POP, 0));
			// pack.instructions.push_back(Instruction(VM_CODE::POP, 0));
		}
	}

	void CodeGen::visit(NumberNode* _node)
	{
		unsigned int index;

		if (_node->maybeInt)
			index = state->AddConstant(Value(static_cast<TSmallInt>(_node->number)));
		else
			index = state->AddConstant(Value(_node->number));

		AddInst(Instruction(VM_CODE::LOAD_C, index));
	}

	void CodeGen::visit(StringNode* _node)
	{
		auto index = state->AddConstant(
			Value(String::FromU16String(_node->content), TypeId::String));

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
			ReportError(std::u16string(u"<Identifier>Variables not found: ") + _node->name);
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
				ReportError(std::u16string(u"<Assignment>Identifier not found: ") + _id_node->name);
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
		auto new_state = GenState::CreateEqualState(state);
		state = new_state;

		int jmp_val;
		visit(_node->condition);
		auto jmp_loc = state->AddInstruction(VM_CODE::IFNO, 1);
		visit(_node->true_branch);
		auto true_finish_loc = state->AddInstruction(VM_CODE::JMP, 1);
		// if condition not ture, jmp to the right location
		jmp_val = state->GetInstructionVector()->size() - jmp_loc;
		(*state->GetInstructionVector())[jmp_loc] = Instruction(VM_CODE::IFNO, jmp_val); 
		if (_node->false_branch)
		{
			visit(_node->false_branch);
			jmp_val = state->GetInstructionVector()->size() - true_finish_loc;
			(*state->GetInstructionVector())[true_finish_loc] = 
				Instruction(VM_CODE::JMP, jmp_val);
		}

		state = state->GetPrevState();
		delete new_state;
	}

	void CodeGen::visit(WhileStmtNode* _node)
	{
		auto new_state = GenState::CreateEqualState(state);
		state = new_state;

		auto _def_vs = _while_statement;
		auto _def_break_loc = _break_loc;
		auto _def_continue_loc = this->_continue_loc;
		_while_statement = true;
		_break_loc = -1;
		this->_continue_loc = -1;

		auto _begin_loc = state->GetInstructionVector()->size();
		visit(_node->condition);
		auto _condition_loc = state->GetInstructionVector()->size();
		state->AddInstruction(VM_CODE::IFNO, 0);
		visit(_node->child);
		state->AddInstruction(VM_CODE::JMP, -1 * 
			(state->GetInstructionVector()->size() - _begin_loc));
		(*state->GetInstructionVector())[_condition_loc] = 
			Instruction(VM_CODE::IFNO, 
				state->GetInstructionVector()->size() - _condition_loc);

		if (_break_loc >= 0)
			(*state->GetInstructionVector())[_break_loc] = 
			Instruction(VM_CODE::JMP, 
				state->GetInstructionVector()->size() - _break_loc);
		if (this->_continue_loc >= 0)
			(*state->GetInstructionVector())[this->_continue_loc] = 
				Instruction(VM_CODE::JMP, 
					_begin_loc - this->_continue_loc);

		_break_loc = _def_break_loc;
		_continue_loc = _def_continue_loc;
		_while_statement = _def_vs;

		state = state->GetPrevState();
		delete new_state;
	}

	void CodeGen::visit(BreakStmtNode* _node)
	{
		if (!_while_statement)
			throw std::logic_error("You should place \"break\" in while statment.");
		_break_loc = state->AddInstruction(VM_CODE::JMP, 0);
	}

	void CodeGen::visit(ContinueStmtNode * _node) 
	{
		if (!_while_statement)
			throw std::logic_error("You should place \"continue\" in while statment.");
		_continue_loc = state->AddInstruction(VM_CODE::JMP, 0);
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

		auto new_state = GenState::CreateNewState(state);

		state = new_state;

		for (auto i = _node->parameters.begin();
			i != _node->parameters.end(); ++i)
			visit(*i);

		visit(_node->block);
		AddInst(Instruction(VM_CODE::RETURN, 0));

		state = new_state->GetPrevState();

		auto new_fun = Context::GetGC()->New<Function>(GenState::GenerateCodePack(new_state));

		delete new_state;

		int const_id = state->AddConstant(new_fun->toValue());
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

		AddInst(Instruction(VM_CODE::PUSH_NULL, 0)); // Push This
		visit(_node->exp);
		AddInst(Instruction(VM_CODE::CALL, _node->parameters.size()));
	}

	CodeGen::~CodeGen()
	{
		if (name != nullptr)
			delete name;
		delete state;
	}

}

#undef TEXT
