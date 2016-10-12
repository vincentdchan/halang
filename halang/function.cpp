#include "function.h"
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

}
