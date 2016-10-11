#pragma once
#include <cinttypes>

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
		DOT_CALL,				// 0x0d
		CALL,					// 0x0e (A, B, C...) call function(A, B, C...)
		RETURN,					// 0x0f A - if A != 0 return exp else return nothing
		IFNO,					// 0x10 if not true, jump to the location.
		JMP,					// 0x11
		NOT,					// 0x12
		ADD,					// 0x13 add the top two val
		SUB,					// 0x14
		MUL,					// 0x15
		DIV,					// 0x16
		MOD,					// 0x17
		POW,					// 0x18
		GT, LT,					// 0x19 0x1a
		GTEQ, LTEQ,				// 0x1b 0x1c
		EQ,						// 0x1d
		OUT,					// 0x1e
		STOP					// 0x1f
	};

	struct Instruction
	{
		std::uint16_t _content_;

		Instruction(VM_CODE _code, int param) : _content_(static_cast<std::int16_t>(_code))
		{
			_content_ = ((_content_ << 8) | (param & 0xFF));
		}
		inline VM_CODE getCode() const
		{
			return static_cast<VM_CODE>((_content_ >> 8) & 0xFF);
		}
		inline std::int8_t getParam() const
		{
			return static_cast<int8_t>(_content_ & 0xFF);
		}
	};

};
