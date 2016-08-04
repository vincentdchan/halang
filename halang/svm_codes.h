#pragma once

namespace halang
{
	enum class VM_CODE
	{
		LOAD_C,					// 0x00 A - load constant
		LOAD_V,					// 0x01 A - load variable
		LOAD_G,					// 0x02 A - load global variable
		LOAD_UPVAL,				// 0x03 A - load upvalue
		STORE_V,				// 0x04 A - store the top of stack to A
		STORE_G,				// 0x05 A - store to the global value
		STORE_UPVAL,			// 0x06 A - store to the upvalue table
		PUSH_INT,				// 0x07 A - load A
		PUSH_BOOL,				// 0x08 A - load A
		POP,					// 0x09 A - Pop A
		CLOSURE,				// 0x0a A - linked Function's upvalue to current env
		CALL,					// 0x0b (A, B, C...) call function(A, B, C...)
		RETURN,					// 0x0c A - if A != 0 return exp else return nothing
		IFNO,					// 0x0d if not true, jump to the location.
		JMP,					// 0x0e
		NOT,					// 0x0f
		ADD,					// 0x10 add the top two val
		SUB,					// 0x11
		MUL,					// 0x12
		DIV,					// 0x13
		MOD,					// 0x14
		POW,					// 0x15
		GT, LT,					// 0x16 0x17
		GTEQ, LTEQ,				// 0x18 0x19
		EQ,						// 0x1a
		OUT,					// 0x1b
		STOP					// 0x1c
	};

};
