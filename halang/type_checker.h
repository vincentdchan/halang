#pragma once
#include "halang.h"
#include "ast.h"
#include "util.h"
#include "type.h"
#include <memory>

namespace halang
{
	class TypeChecker : public utils::_MessageContainer
	{
	public:
		TypeChecker() : guestFuncType(false), env(nullptr)
		{

		}

		// disable copy constructor
		TypeChecker(TypeChecker&) = delete;
		TypeChecker(TypeChecker&&) = delete;
		TypeChecker& operator=(TypeChecker&) = delete;

		static std::unique_ptr<TypeChecker> TypeCheck(Node* node);

		virtual ~TypeChecker()
		{
			if (env != nullptr)
				delete env;
		}

	private:

		struct TypeCheckEnv;		// TypeCheck Environment

		static void TypeCheck(TypeChecker& tc, Node* node);
#define VISIT_METHOD(NAME) static void TypeCheck(TypeChecker& tc, NAME##Node*);
		NODE_LIST(VISIT_METHOD)
#undef VISIT_METHOD

		static Type getTypeFromString(TypeChecker* tc, std::string typeName);

		Type mustReturnType;
		Type currentType;
		std::unique_ptr<Type> guestType;
		TypeCheckEnv* env;

		bool guestFuncType;
	};

	struct TypeChecker::TypeCheckEnv
	{
		TypeCheckEnv() : prev(nullptr)
		{}
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
