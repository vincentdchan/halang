#pragma once
#include "halang.h"
#include "object.h"
#include "GC.h"
#include "upvalue.h"

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
		ScriptContext(const ScriptContext&);

	private:

		ScriptContext* prev;

		Value* stack;
		size_type stack_size;

		Value* sptr;

		Value* variables;
		size_type variable_size;

		UpValue** upvals;
		size_type upvals_size;

		bool cp;

	public:

		inline Value Top(int i = 0);
		inline Value Pop();
		inline void Push(Value v);
		inline Value GetVariable(unsigned int i);
		inline UpValue* GetUpValue(unsigned int i);
		inline void SetVariable(unsigned int i, Value);
		inline void SetUpValue(unsigned int i, UpValue*);
		void CloseAllUpValue();

		virtual ~ScriptContext();
	};

}
