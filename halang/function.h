#pragma once
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include "svm.h"
#include "string.h"
#include "object.h"
#include "upvalue.h"
#include "svm_codes.h"

namespace halang
{

	class CodePack : public Object
	{

	};

	class Function : public Object
	{
	public:
		friend class GC;

		class FunctionArgs;
		class ReturnValue;

		typedef std::function<void (const FunctionArgs&, ReturnValue&)> ExternFunction;

		// how to call a hafunction
		void Call(Object* target, const FunctionArgs&, const ReturnValue&);

	protected:

		Function() : isExtern(false)
		{
			typeId = TypeId::Function;
		}

		Function(const ExternFunction& _fun) : isExtern(true), externFunc(_fun)
		{}

	private:

		String* name;

		bool isExtern;

		union 
		{
			CodePack* cp;
			ExternFunction externFunc;
		};

	};

};
