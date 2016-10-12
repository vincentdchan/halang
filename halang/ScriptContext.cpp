#include "ScriptContext.h"
#include "halang.h"

namespace halang
{

	ScriptContext::ScriptContext(size_type _var_size, size_type _upval_size):
		variable_size(_var_size), upvals_size(_upval_size)
	{
		stack_size = VM_STACK_SIZE;
		stack = new Value[stack_size]();

		sptr = stack;

		var_ptr = variables = new Value[variable_size];

		upval_ptr = upvals = new UpValue*[upvals_size];

	}

	ScriptContext::ScriptContext(Function * _fun)
	{

		stack_size = VM_STACK_SIZE;
		stack = new Value[stack_size];

		sptr = stack;

		if (_fun->isExtern)
		{
			variable_size = 0;
			upvals_size = 0;
			var_ptr = variables = nullptr;
			upval_ptr = upvals = nullptr;
		}
		else
		{
			CodePack * cp = _fun->codepack;

			variable_size = cp->_var_names_size;
			upvals_size = cp->_upval_names_size;

			var_ptr = variables = new Value[variable_size]();
			upval_ptr = upvals = new UpValue*[upvals_size]();

			saved_ptr = cp->_instructions;
		}

		function = _fun;

	}

	ScriptContext::ScriptContext(const ScriptContext& sc):
		stack_size(sc.stack_size), variable_size(sc.variable_size),
		upvals_size(sc.upvals_size)
	{

		stack = new Value[stack_size];
		for (unsigned int i = 0; i < stack_size; ++i)
			stack[i] = sc.stack[i];

		variables = new Value[variable_size];
		for (unsigned int i = 0; i < variable_size; ++i)
			variables[i] = sc.variables[i];

		upvals = new UpValue*[upvals_size];
		for (unsigned int i = 0; i < upvals_size; ++i)
			upvals[i] = sc.upvals[i];

		sptr = stack + (sc.sptr - sc.stack);
		var_ptr = variables + (sc.var_ptr - sc.variables);
		upval_ptr = upvals + (sc.upval_ptr - sc.upvals);

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

	void ScriptContext::PushUpValue(UpValue * uv)
	{
		*(upval_ptr++) = uv;
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