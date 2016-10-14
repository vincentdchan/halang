#include "function.h"
#include "ScriptContext.h"
#include "upvalue.h"
#include <sstream>

namespace halang
{

	std::string Function::ToString(Function* fun)
	{
		std::stringstream ss;
		ss << "<Function>" << std::endl;
		if (fun->isExtern)
		{
			ss << "Extern Function" << std::endl;
		}
		else
		{
			ss << "CodePack" << std::endl;
			CodePack * cp = fun->codepack;
			for (unsigned int i = 0; i < cp->_instructions_size; ++i)
				ss << Instruction::ToString(cp->_instructions[i]) << std::endl;
		}
		return ss.str();
	}

	void CodePack::Mark()
	{
		if (!marked)
		{
			marked = true;
			if (prev != nullptr)
				prev->Mark();

			for (size_type i = 0; i < _const_size; ++i)
				if (_constants[i].isGCObject())
					_constants[i].value.gc->Mark();

			for (size_type i = 0; i < _var_names_size; ++i)
				_var_names[i]->Mark();

			for (size_type i = 0; i < _upval_names_size; ++i)
				_upval_names[i]->Mark();
		}
	}

	void FunctionArgs::Mark()
	{
		if (!marked)
		{
			marked = true;
			Array::Mark();
			scriptContext->Mark();
		}
	}

	void Function::Mark()
	{
		if (!marked)
		{
			marked = true;
			if (!isExtern)
				codepack->Mark();

			if (name != nullptr)
				name->Mark();

			if (thisOne.isGCObject())
				thisOne.value.gc->Mark();

			for (auto i = upvalues.begin(); i != upvalues.end(); ++i)
				(*i)->Mark();
		}
	}

}
