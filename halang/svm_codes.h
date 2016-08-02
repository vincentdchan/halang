#pragma once

namespace halang
{
	enum class VM_CODE
	{
		LOAD_C,					// A - load constant
		LOAD_V,					// A - load variable
		LOAD_G,					// A - load global variable
		LOAD_UPVAL,				// A - load upvalue
		STORE_V,				// A - store the top of stack to A
		STORE_G,				// A - store to the global value
		STORE_UPVAL,			// A - store to the upvalue table
		PUSH_INT,				// A - load A
		PUSH_BOOL,				// A - load A
		POP,					// A - Pop A
		CLOSURE,				// A - linked Function's upvalue to current env
		CALL,					// (A, B, C...) call function(A, B, C...)
		RETURN,					// return
		IFNO,					// if not true, jump to the location.
		JMP,
		NOT,
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

};
