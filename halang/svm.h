#pragma once
#include <iostream>
#include <cinttypes>
#include <utility>
#include "ScriptContext.h"
#include "string_pool.h"
#include "svm_codes.h"
#include "object.h"
#include "upvalue.h"
#include "context.h"
#include "GC.h"

#define PRE(POINTER) ((POINTER) - 1)

namespace halang
{
	class CodePack;

	struct Instruction
	{
		uint16_t _content_;
		Instruction(VM_CODE _code, int param) : _content_(static_cast<int16_t>(_code))
		{
			_content_ = ((_content_ << 8) | (param & 0xFF));
		}
		inline VM_CODE getCode() const
		{
			return static_cast<VM_CODE>((_content_ >> 8) & 0xFF);
		}
		inline int8_t getParam() const
		{
			return static_cast<int8_t>(_content_ & 0xFF);
		}
	};

	typedef std::vector<Instruction>::iterator InstIter;

	class StackVM final
	{
	private:

		GC gc;
		unsigned int* frame_ptr;

	public:

		StackVM() : gc()
		{
			Context::vm = this;
		}

	};

}
