#pragma once
#include <map>
#include <vector>
#include "ast.h"
#include "util.h"
#include "parser.h"
#include "svm.h"
#include "function.h"
#include "object.h"
#include "ScriptContext.h"
#include "visitor.h"

namespace halang
{

	/// <summary>
	/// CodeGen create a absolute function represent 
	/// the whole program. This absolute function includes
	/// many default variable and funtions. The sub-function
	/// can refer to this object by upvalues
	///
	/// At last, this absolute function can be pass to the 
	/// VM, then execute it.
	/// </summary>
	class CodeGen final : 
		public utils::_MessageContainer<std::u16string>, public Visitor
	{
	public:
		class GenState;
		class VarType;

		CodeGen();

		// CodeGen do not allow copy
		CodeGen(const CodeGen&) = delete;
		CodeGen(CodeGen&&) = delete;
		CodeGen& operator=(const CodeGen&) = delete;
		CodeGen& operator=(CodeGen&&) = delete;

		~CodeGen();
		void generate(Parser*);

		void InitFinalFunction();

		void PushState();

		void PopState();

		void load();

		VarType FindVar(const std::u16string&);

		void AddInstruction(Instruction ins);

		virtual void visit(Node*) override;
#define VISIT_METHOD(NAME) void visit(NAME##Node*);
		NODE_LIST(VISIT_METHOD)
#undef VISIT_METHOD


	private:

		Function* final_function;

		Parser* parser;

		GenState* state;

		bool _var_statment;

	};

	class CodeGen::GenState
	{
	public:
		friend class CodeGen;

	protected:

		GenState* prev;

		std::vector<Object *> constant;
		std::vector<std::u16string> variable_names;
		std::vector<std::u16string> upvalue_names;
		std::vector<std::uint32_t> required_upval_ids;

		std::vector<Instruction> instructions;

		GenState() :
			prev(nullptr)
		{}

	public:

		CodePack* Finish();

	};

	class CodeGen::VarType
	{
	public:
		enum TYPE
		{
			LOCAL,
			GLOBAL,
			UPVAL,
			NONE
		};
		VarType(TYPE _t = NONE, int _d = 0) : _type(_t), _id(_d)
		{}

		inline TYPE type() const { return _type; }
		inline int id() const { return _id; }
	private:
		TYPE _type;
		int _id;
	};

};
