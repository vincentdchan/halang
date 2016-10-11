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

		ScriptContext(size_type _var_size, size_type _upval_size);
		ScriptContext(Function * );
		ScriptContext(const ScriptContext&);

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

		UpValue** upvals;
		UpValue** upval_ptr;
		size_type upvals_size;

		bool cp;

	public:

		inline Value Top(int i = 0);
		inline Value Pop();
		inline void Push(Value v);
		inline Value GetConstant(unsigned int i);
		inline Value GetVariable(unsigned int i);
		inline UpValue* GetUpValue(unsigned int i);
		inline void PushUpValue(UpValue*);
		inline void SetVariable(unsigned int i, Value);
		inline void SetUpValue(unsigned int i, UpValue*);
		void CloseAllUpValue();

		virtual ~ScriptContext();
	};

}
