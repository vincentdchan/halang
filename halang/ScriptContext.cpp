#include "ScriptContext.h"
#include "halang.h"
#include <cstdlib>

namespace halang
{

	/*
	ScriptContext::ScriptContext(size_type _var_size, size_type _upval_size):
		variable_size(_var_size), upvals_size(_upval_size)
	{
		stack_size = VM_STACK_SIZE;
		stack = new Value[stack_size]();

		sptr = stack;

		var_ptr = variables = new Value[variable_size];

		upval_ptr = upvals = new UpValue*[upvals_size];

	}
	*/

	ScriptContext::ScriptContext(Function * _fun) :
		function(nullptr), prev(nullptr), stack(nullptr), sptr(nullptr),
		variables(nullptr), var_ptr(nullptr), upvals(nullptr),
		stack_size(0), variable_size(0), upvals_size(0)
	{

		stack_size = VM_STACK_SIZE;
		stack = new Value[stack_size];

		sptr = stack;

		if (_fun->isExtern)
		{
			variable_size = 0;
			upvals_size = 0;
			var_ptr = variables = nullptr;
			upvals = nullptr;
		}
		else
		{
			CodePack * cp = _fun->codepack;

			variable_size = cp->_var_names_size;
			upvals_size = cp->_upval_names_size;

			var_ptr = variables = new Value[variable_size]();

			/*
			if (cp->_require_upvalues_size > 0)
				upvals = new UpValue*[cp->_require_upvalues_size]();
				*/
			if (_fun->upvalues.size() > 0) {
				upvals = new UpValue*[_fun->upvalues.size()]();
				for (unsigned int i = 0;
					i < _fun->upvalues.size(); ++i)
					upvals[i] = _fun->upvalues[i];
				upvals_size = _fun->upvalues.size();
			}

			saved_ptr = cp->_instructions;
		}

		function = _fun;
	}

	ScriptContext::ScriptContext(const ScriptContext& sc):
		stack_size(sc.stack_size), variable_size(sc.variable_size),
		upvals_size(sc.upvals_size)
	{

		stack = new Value[stack_size]();
		for (unsigned int i = 0; i < stack_size; ++i)
			stack[i] = sc.stack[i];

		variables = new Value[variable_size]();
		for (unsigned int i = 0; i < variable_size; ++i)
			variables[i] = sc.variables[i];

		upvals = new UpValue*[upvals_size]();
		for (unsigned int i = 0; i < upvals_size; ++i)
			upvals[i] = sc.upvals[i];

		sptr = stack + (sc.sptr - sc.stack);
		var_ptr = variables + (sc.var_ptr - sc.variables);
		upvals_size = sc.upvals_size;

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

	Value ScriptContext::GetVariable(unsigned int i) const
	{
		return variables[i];
	}

	UpValue* ScriptContext::GetUpValue(unsigned int i) const
	{
		return upvals[i];
	}

	void ScriptContext::PushUpValue(UpValue * uv)
	{
		upvals[upvals_size++] = uv;
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

	void ScriptContext::Mark()
	{
		if (!marked)
		{
			marked = true;
			function->Mark();

			if (prev != nullptr)
				prev->Mark();

			Value* t = stack;
			while (t != sptr)
			{
				if (t->isGCObject())
					t->value.gc->Mark();
				t++;
			}

			for (size_type i = 0; i < variable_size; ++i)
				if (variables[i].isGCObject())
					variables[i].value.gc->Mark();

			for (size_type i = 0; i < upvals_size; ++i)
				if (upvals[i] != nullptr)
					upvals[i]->Mark();
		}
	}

	ScriptContext::~ScriptContext()
	{
		delete[] stack;
		if (variables != nullptr)
			delete[] variables;
		if (upvals != nullptr)
			delete[] upvals;
	}

}