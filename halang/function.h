#pragma once
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <cinttypes>
#include "svm.h"
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
		typedef std::uint32_t size_type;

		static const size_type DEFAULT_CAPACITY = 64;

	private:

		Instruction* base;
		size_type capacity;
		size_type length;

	protected:

		CodePack() :
			capacity(DEFAULT_CAPACITY), length(0)
		{
			typeId = TypeId::CodePack;

			base = Context::GetGc()->NewInstructionArray(capacity);
		}

		void raiseSize()
		{
			auto new_size = capacity * 2;
			auto new_ptr = Context::GetGc()->NewInstructionArray(new_size);

			for (size_type i = 0; i < length; ++i)
			{
				new_ptr[i] = base[i];
			}

			base = new_ptr;
			capacity = new_size;
		}

	public:

		inline size_type GetLength() const
		{
			return length;
		}

		inline void AddInstruction(Instruction ins)
		{
			if (length >= capacity)
				raiseSize();
			base[length++] = ins;
		}

		inline Instruction GetInstuction(unsigned int index)
		{
			if (index >= length)
				throw std::runtime_error("index out of instructions bound");
			return base[index];
		}

		inline void SetInstruction(unsigned int index, Instruction ins)
		{
			if (index >= capacity)
				throw std::runtime_error("index out of capacity");
			base[index] = ins;
			if (index >= length)
				length = index + 1;
		}

		virtual void Mark() override
		{
			GC::SetMark(base);
		}

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

	private:

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
			isExtern(false), script_context(nullptr)
		{
			typeId = TypeId::Function;
		}

		Function(const ExternFunction& _fun) : 
			isExtern(true), externFunc(_fun),
			script_context(nullptr)
		{}

	};

};
