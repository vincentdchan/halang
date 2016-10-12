#pragma once
#include <cinttypes>
#include <sstream>

#define SVM_CODES(V) \
	V(LOAD_C,			0x00) \
	V(LOAD_V,			0x01) \
	V(LOAD_G,			0x02) \
	V(LOAD_UPVAL,		0x03) \
	V(STORE_V,			0x04) \
	V(STORE_G,			0x05) \
	V(STORE_UPVAL,		0x06) \
	V(SET_VAL,			0x07) \
	V(GET_VAL,			0x08) \
	V(PUSH_INT,			0x09) \
	V(PUSH_BOOL,		0x0a) \
	V(POP,				0x0b) \
	V(CLOSURE,			0x0c) \
	V(DOT_CALL,			0x0d) \
	V(CALL,				0x0e) \
	V(RETURN,			0x0f) \
	V(IFNO,				0x10) \
	V(JMP,				0x11) \
	V(NOT,				0x12) \
	V(ADD,				0x13) \
	V(SUB,				0x14) \
	V(MUL,				0x15) \
	V(DIV,				0x16) \
	V(MOD,				0x17) \
	V(POW,				0x18) \
	V(GT,				0x19) \
	V(LT,				0x1a) \
	V(GTEQ,				0x1b) \
	V(LTEQ,				0x1c) \
	V(EQ,				0x1d) \
	V(OUT,				0x1e) \
	V(STOP,				0x1f)

namespace halang
{
#define CC(NAME, CODE) NAME = CODE ,
	enum class VM_CODE
	{
		SVM_CODES(CC)
	};
#undef CC

	struct Instruction
	{
		std::uint16_t _content_;

		Instruction()
		{
			_content_ = 0;
		}

		Instruction(VM_CODE _code, int param) : _content_(static_cast<std::int16_t>(_code))
		{
			_content_ = ((_content_ << 8) | (param & 0xFF));
		}

		Instruction(const Instruction& _another)
		{
			_content_ = _another._content_;
		}

		inline VM_CODE GetCode() const
		{
			return static_cast<VM_CODE>((_content_ >> 8) & 0xFF);
		}

		inline std::int8_t GetParam() const
		{
			return static_cast<int8_t>(_content_ & 0xFF);
		}

	private:

		static const char* CodeToString(VM_CODE vc)
		{
#define CC(NAME, CODE) \
case VM_CODE::NAME: \
			return #NAME ;

			switch (vc)
			{
				SVM_CODES(CC)
			default:
				return "";
			}
		}
#undef CC

	public:

		static std::string ToString(const Instruction& inst)
		{
			std::stringstream ss;
			ss << CodeToString(inst.GetCode()) << "\t" << static_cast<int>(inst.GetParam());
			return ss.str();
		}

	};

};
