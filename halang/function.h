#pragma once
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <cinttypes>
#include "svm.h"
#include "Array.h"
#include "string.h"
#include "object.h"
#include "upvalue.h"
#include "svm_codes.h"
#include "ScriptContext.h"

namespace halang
{

	class CodePack : public Object
	{
	public:

		friend class GC;
		friend class StackVM;
		friend class CodeGen;

		typedef std::uint32_t size_type;

	private:

		Array* constant;

		size_type var_len;
		size_type upval_len;

		String** var_names;
		String** upval_names;

		Instruction* instructions;
		size_type instruction_length;

		std::uint32_t* required_upval_ids;
		size_type required_upval_ids_len;

	protected:

		CodePack() : instruction_length(0)
		{
			typeId = TypeId::CodePack;

			constant = Context::GetGc()->New<Array>();
		}


	public:

		inline size_type GetLength() const
		{
			return length;
		}

		inline Instruction GetInstuction(unsigned int index)
		{
			if (index >= length)
				throw std::runtime_error("index out of instructions bound");
			return instructions[index];
		}

		virtual void Mark() override
		{
			GC::SetMark(instructions);
		}

	};

	class Function : public Object
	{
	public:
		friend class StackVM;
		friend class GC;
		friend class CodeGen;

		class FunctionArgs;
		class ReturnValue;

		typedef std::uint32_t size_type;
		typedef std::function<void (const FunctionArgs&, ReturnValue&)> ExternFunction;

		// how to call a hafunction
		void Call(Object* target, const FunctionArgs&, const ReturnValue&);

	private:

		size_type required_params_size;

		ScriptContext* script_context;

		String* name;

		bool isExtern;

		union 
		{
			CodePack* cp;
			ExternFunction externFunc;
		};

	protected:

		Function() :
			isExtern(false), script_context(nullptr),
			required_params_size(0)
		{
			typeId = TypeId::Function;
		}

		Function(const ExternFunction& _fun) : 
			isExtern(true), externFunc(_fun),
			script_context(nullptr)
		{}

	};

};
