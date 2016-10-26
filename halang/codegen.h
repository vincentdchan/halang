#pragma once
#include <map>
#include <vector>
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
		public utils::_MessageContainer<std::u16string>, public Visitor
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

		virtual void visit(Node*) override;
#define VISIT_METHOD(NAME) void visit(NAME##Node*);
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

		friend class CodeGen;

		typedef unsigned int size_type;

		GenState(GenState* _prev = nullptr):
			prev(_prev)
		{}

		static CodePack* GenerateCodePack(GenState*);

	private:

		GenState* prev;

		size_type params_size;
		std::vector<Value> constant;
		std::vector<std::u16string> var_names;
		std::vector<std::u16string> upvalue_names;
		std::vector<int> require_upvalues;
		std::vector<Instruction> instructions;

	public:

		size_type AddVariable(const std::u16string& name)
		{
			size_type i = var_names.size();
			var_names.push_back(name);
			return i;
		}

		size_type AddUpValue(const std::u16string& name)
		{
			size_type i = upvalue_names.size();
			upvalue_names.push_back(name);
			return i;
		}

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

};
