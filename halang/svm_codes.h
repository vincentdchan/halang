#pragma once

namespace runtime
{
	namespace StackVM
	{
		enum class VM_CODE
		{
			LOAD_V,					// A load variable
			LOAD_C,					// A load constant
			STORE_V,					// A store the top of stack to A
			PUSH_INT,				// A load A
			PUSH_BOOL,				// A load A
			POP,					// Pop
			IFNO,					// if not true, jump to the location.
			JMP,
			ADD,					// add the top two val
			SUB,
			MUL,
			DIV,
			MOD,
			POW,
			GT, LT,
			GTEQ, LTEQ,
			EQ,
			OUT,
			STOP
		};
	}
}
