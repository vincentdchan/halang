#pragma once
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include "ast.h"
#include "util.h"
#include "parser.h"
#include "svm.h"
#include "function.h"
#include "object.h"
#include "visitor.h"

namespace halang
{

	class CodeGen final : 
		public utils::MessageContainer, public Visitor
	{
	public:

		class GenState;
		class VarType;

		CodeGen(StackVM *_vm);
		CodeGen(const CodeGen&) = delete;
		CodeGen(CodeGen&&) = delete;
		CodeGen& operator=(const CodeGen&) = delete;
		CodeGen& operator=(CodeGen&&) = delete;
		~CodeGen();

		Function* generate(Parser*);

		static VarType FindVar(GenState*, const std::u16string&);

		virtual void Visit(Node*) override;
#define VISIT_METHOD(NAME) void Visit(NAME##Node*);
		NODE_LIST(VISIT_METHOD)
#undef VISIT_METHOD


	private:

		std::u16string* name;

		bool _var_statement;
		bool _while_statement;
		int _break_loc;
		int _continue_loc;

		StackVM* vm;
		Parser* parser;
		GenState* state;

		GenState* GenerateDefaultState();
		void AddInst(Instruction i);

	};

	class CodeGen::GenState
	{
	public:

		static const unsigned int ENTRY_SIZE = 64;
		static GenState* CreateEqualState(GenState*);
		static GenState* CreateNewState(GenState* prev = nullptr);
		static CodePack* GenerateCodePack(GenState*);

		class VariableEntry;

		typedef std::shared_ptr<VariableEntry> pEntry;
		typedef unsigned int size_type;


	private:

		GenState() :
			prev(nullptr), father_state(nullptr),
			var_names_entries(ENTRY_SIZE)
		{}
		GenState(const GenState&) = delete;
		GenState& operator=(const GenState&) = delete;

		int _level;

		void copyEntry()
		{
			for (unsigned int i = 0; i < prev->var_names_entries.size();
				i++) 
			{
				if (prev->var_names_entries[i] != nullptr) 
				{
					this->var_names_entries[i] = prev->var_names_entries[i];
				}
			}
		}

		std::vector<pEntry> var_names_entries;

		unsigned int _var_names_size;
		unsigned int* _max_entries_size;
		bool isNew;

		GenState *prev, *father_state;

		size_type params_size;

		std::vector<std::u16string>* upvalue_names;
		std::vector<int>* require_upvalues;

	public:

		std::vector<Value>* constant;
		std::vector<Instruction>* instructions;

		bool ExistName(const std::u16string& _name) const;

		bool TryGetVarId(const std::u16string& _name, int &_id) const;

		size_type AddVariable(const std::u16string& name);

		size_type size();

		size_type AddUpValue(const std::u16string& name);

		void forEachVarNames(std::function<void(const std::u16string&, int)> _fun);

		inline std::vector<std::u16string>* GetUpValuesVector() const 
		{
			return upvalue_names;
		}

		inline std::vector<int>* GetRequireUpvaluesVector() const
		{
			return require_upvalues;
		}

		inline size_type VarNamesSize() const
		{
			return _var_names_size;
		}

		inline size_type MaxVarNamesSize() const
		{
			return *_max_entries_size;
		}
		
		inline GenState* GetFather() const
		{
			return father_state;
		}

		inline GenState* GetPrevState() const
		{
			return prev;
		}

		inline std::vector<Instruction>* 
			GetInstructionVector() const
		{
			return instructions;
		}

		inline std::vector<Value>*
			GetConstantVector() const
		{
			return constant;
		}

		std::vector<Instruction>::size_type 
			AddInstruction(VM_CODE code, int param)
		{
			auto _size = instructions->size();
			instructions->push_back(Instruction(code, param));
			return _size;
		}

		std::vector<Instruction>::size_type 
			AddInstruction(const Instruction& inst)
		{
			auto _size = instructions->size();
			instructions->push_back(inst);
			return _size;
		}

		std::vector<Value>::size_type
			AddConstant(Value _value)
		{
			auto _size = constant->size();
			constant->push_back(_value);
			return _size;
		}

		~GenState();

	};

	class CodeGen::VarType
	{
	public:

		enum class TYPE
		{
			LOCAL,
			GLOBAL,
			UPVAL,
			NONE
		};

		VarType(TYPE _t = TYPE::NONE, int _d = 0): 
			_type(_t), _id(_d)
		{}

		inline TYPE type() const { return _type; }
		inline int id() const { return _id; }

	private:
		TYPE _type;
		int _id;
	};

	class CodeGen::GenState::VariableEntry
	{
	public:

		VariableEntry(std::u16string _name, 
			int _id = -1, bool _fast = true) :
			isFastVar(_fast), name(_name), givenId(_id)
		{
			hash = std::hash<std::u16string>{}(name);
		}

		std::shared_ptr<VariableEntry> next;
		unsigned int hash;
		bool isFastVar;
		std::u16string name;
		int givenId;
	};

};
