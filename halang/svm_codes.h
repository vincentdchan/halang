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
		SET_VAL,				// 0x07   - pop the top three of stack as dict, key and value
		GET_VAL,				// 0x08	  - pop the top two of stack as dict and key
		PUSH_INT,				// 0x09 A - load A
		PUSH_BOOL,				// 0x0a A - load A
		POP,					// 0x0b A - Pop A
		CLOSURE,				// 0x0c A - linked Function's upvalue to current env
		CALL,					// 0x0d (A, B, C...) call function(A, B, C...)
		RETURN,					// 0x0e A - if A != 0 return exp else return nothing
		IFNO,					// 0x0f if not true, jump to the location.
		JMP,					// 0x10
		NOT,					// 0x11
		ADD,					// 0x12 add the top two val
		SUB,					// 0x13
		MUL,					// 0x14
		DIV,					// 0x15
		MOD,					// 0x16
		POW,					// 0x17
		GT, LT,					// 0x18 0x19
		GTEQ, LTEQ,				// 0x1a 0x1b
		EQ,						// 0x1c
		OUT,					// 0x1d
		STOP					// 0x1e
	};

};
