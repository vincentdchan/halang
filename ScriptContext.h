#pragma once
#include "halang.h"
#include "object.h"
#include "GC.h"
#include "upvalue.h"
#include "function.h"
#include "svm_codes.h"

namespace halang
{

	class ScriptContext : public GCObject
	{
	public:
		
		friend class StackVM;
		friend class GC;
		typedef unsigned int size_type;

	protected:

		// ScriptContext(size_type _var_size, size_type _upval_size);
		ScriptContext(Function * );
		ScriptContext(const ScriptContext&);

		ScriptContext& operator=(const ScriptContext&) = delete;

	private:

		Function* function;
		Instruction* saved_ptr;

		ScriptContext* prev;

		Value* stack;
		Value* sptr;
		size_type stack_size;


		Value* variables;
		Value* var_ptr;
		size_type variable_size;

		/// <summary>
		/// host upvalues means the upvalue created in this SriptContext
		/// when this ScriptContext is closing, all the upvalues would be
		/// closed.
		/// </summary>
		std::vector<UpValue*> host_upvals;

		bool cp;

	public:

		Value Top(int i = 0);
		Value Pop();
		void Push(Value v);
		// Value GetConstant(unsigned int i);
		Value GetVariable(unsigned int i) const;
		UpValue* GetUpValue(unsigned int i) const;
		// void PushUpValue(UpValue*);
		void SetVariable(unsigned int i, Value);
		void SetUpValue(unsigned int i, UpValue*);
		void CloseAllUpValue();

		virtual void Mark() override;

		virtual ~ScriptContext();
	};

}
