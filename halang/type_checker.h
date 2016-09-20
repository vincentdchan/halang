#pragma once
#include "halang.h"
#include "ast.h"
#include "util.h"
#include "type.h"
#include "visitor.h"
#include <memory>


namespace halang
{

	/// <summary>
	/// Do the type check to the root of AST.
	/// </summary>
	class TypeChecker : 
		public Visitor, public utils::_MessageContainer<std::string>
	{
	private:
		struct TypeCheckEnv;		// TypeCheck Environment

	public:

		TypeChecker();

		// disable copy constructor
		TypeChecker(TypeChecker&) = delete;
		TypeChecker(TypeChecker&&) = delete;
		TypeChecker& operator=(TypeChecker&) = delete;

		virtual ~TypeChecker()
		{
			if (env != nullptr)
				delete env;
		}

		static Type getTypeFromString(std::string typeName);
		static TypeChecker* TypeCheck(Node *);

		virtual void visit(Node *node) override { node->visit(this); }
#define E(Name) virtual void visit(Name##Node *node) override;
		NODE_LIST(E)
#undef E

	private:
		Type mustReturnType;
		Type currentType;
		std::unique_ptr<Type> guestType;
		TypeCheckEnv* env;

		bool guestFuncType;
	};

	/// <summary>
	/// The envrionment to help the type check.
	/// To simulate the function call.
	/// </summary>
	struct TypeChecker::TypeCheckEnv
	{
	public:
		TypeCheckEnv() : prev(nullptr)
		{}
		~TypeCheckEnv() {}

		TypeCheckEnv* prev;
		std::vector<std::pair<std::string, Type> > vars;

		bool findVar(std::string& _name, std::pair<std::string, Type>& result)
		{
			for (auto i = vars.begin(); i != vars.end(); ++i)
			{ 
				if (i->first == _name)
				{
					result = *i;
					return true;
				}
			}
			if (prev == nullptr)
				return false;
			else
				return prev->findVar(_name, result);
		}
	};

}
