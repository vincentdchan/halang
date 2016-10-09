#include "ScriptContext.h"
#include "halang.h"

namespace halang
{

	ScriptContext::ScriptContext(size_type _var_size, size_type _upval_size):
		variable_size(_var_size), upvals_size(_upval_size)
	{
		stack_size = VM_STACK_SIZE;
		stack = new Value[stack_size];

		sptr = stack;

		variables = new Value[variable_size];

		upvals = new UpValue*[upvals_size];

	}

	ScriptContext::ScriptContext(const ScriptContext& sc):
		stack_size(sc.stack_size), variable_size(sc.variable_size),
		upvals_size(sc.upvals_size)
	{

		stack = new Value[stack_size];
		for (unsigned int i = 0; i < stack_size; ++i)
			stack[i] = sc.stack[i];

		sptr = stack + (sc.sptr - sc.stack);

		variables = new Value[variable_size];
		for (unsigned int i = 0; i < variable_size; ++i)
			variables[i] = sc.variables[i];

		upvals = new UpValue*[upvals_size];
		for (unsigned int i = 0; i < upvals_size; ++i)
			upvals[i] = sc.upvals[i];

	}

	Value ScriptContext::Top(int i)
	{
		return *(sptr - 1 - i);
	}

	Value ScriptContext::Pop()
	{
		return *(--sptr);
	}

	void ScriptContext::Push(Value v)
	{
		*(sptr++) = v;
	}

	Value ScriptContext::GetVariable(unsigned int i)
	{
		return variables[i];
	}

	UpValue* ScriptContext::GetUpValue(unsigned int i)
	{
		return upvals[i];
	}

	void ScriptContext::SetVariable(unsigned int i, Value v)
	{
		variables[i] = v;
	}

	void ScriptContext::SetUpValue(unsigned int i, UpValue* uv)
	{
		upvals[i] = uv;
	}

	void ScriptContext::CloseAllUpValue()
	{
		for (unsigned int i = 0; i < upvals_size; ++i)
			upvals[i]->close();
	}

	ScriptContext::~ScriptContext()
	{
		delete[] stack;
		delete[] variables;
		delete[] upvals;
	}

}