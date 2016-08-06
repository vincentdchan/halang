#pragma once
#include <map>
#include <vector>
#include "ast.h"
#include "util.h"
#include "parser.h"
#include "svm.h"
#include "function.h"
#include "object.h"

namespace halang
{

	class CodeGen final : public utils::_MessageContainer
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
		void generate(Parser*);

		void load();
		void visit(CodePack*, Node*);

		static VarType findVar(CodePack*, IString);
#define VISIT_METHOD(NAME) void visit(CodePack*, NAME##Node*);
		NODE_LIST(VISIT_METHOD)
#undef VISIT_METHOD


	private:

		StackVM* vm;
		Parser* parser;
		CodePack *top_cp, *global_cp;
		GenState* state;


		void generate(CodePack*);

	};

	class CodeGen::GenState
	{
	public:
		GenState() : _var_statement(false)
		{}
		inline bool varStatement() { return _var_statement; }
		inline void setVarStatement(bool _vl) { _var_statement = _vl; }
	private:
		bool _var_statement;
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
