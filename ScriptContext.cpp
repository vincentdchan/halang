#include "ScriptContext.h"
#include "halang.h"
#include <cstdlib>

namespace halang
{

	ScriptContext::ScriptContext(Function * _fun) :
		function(nullptr), prev(nullptr), stack(nullptr), sptr(nullptr),
		variables(nullptr), var_ptr(nullptr),
		stack_size(0), variable_size(0)
	{

		stack_size = VM_STACK_SIZE;
		stack = new Value[stack_size];

		sptr = stack;

		if (_fun->isExtern)
		{
			variable_size = 0;
			var_ptr = variables = nullptr;
		}
		else
		{
			CodePack * cp = _fun->codepack;

			variable_size = cp->_var_names_size;

			var_ptr = variables = new Value[variable_size]();

			saved_ptr = cp->_instructions;
		}

		function = _fun;
	}

	ScriptContext::ScriptContext(const ScriptContext& sc):
		stack_size(sc.stack_size), variable_size(sc.variable_size)
	{

		stack = new Value[stack_size]();
		for (unsigned int i = 0; i < stack_size; ++i)
			stack[i] = sc.stack[i];

		variables = new Value[variable_size]();
		for (unsigned int i = 0; i < variable_size; ++i)
			variables[i] = sc.variables[i];

		sptr = stack + (sc.sptr - sc.stack);
		var_ptr = variables + (sc.var_ptr - sc.variables);

		host_upvals = sc.host_upvals;
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
		if (sptr - stack >= stack_size)
			throw std::runtime_error("stack overflow");
		*(sptr++) = v;
	}

	Value ScriptContext::GetVariable(unsigned int i) const
	{
		return variables[i];
	}

	UpValue* ScriptContext::GetUpValue(unsigned int i) const
	{
		return function->upvalues[i];
	}

	void ScriptContext::SetVariable(unsigned int i, Value v)
	{
		variables[i] = v;
	}

	void ScriptContext::SetUpValue(unsigned int i, UpValue* uv)
	{
		function->upvalues[i] = uv;
	}

	void ScriptContext::CloseAllUpValue()
	{
		for (auto i = host_upvals.begin(); 
			i != host_upvals.end(); i++)
			(*i)->close();
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

			for (auto i = host_upvals.begin(); 
				i != host_upvals.end(); i++)
				(*i)->Mark();
		}
	}

	ScriptContext::~ScriptContext()
	{
		delete[] stack;
		if (variables != nullptr)
			delete[] variables;
	}

}