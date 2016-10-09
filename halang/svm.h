#pragma once
#include <iostream>
#include <vector>
#include <cinttypes>
#include <utility>
#include <map>
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

	struct Environment
	{

		const static unsigned int STACK_SIZE = 255;

		Environment(CodePack* cp);
		Environment* prev;
		Value* stack;
		Value* sptr;

		bool cp;
		InstIter iter;
		Value* variables;
		Value* upvalues;
		CodePack* codepack;
		std::vector<UpValue*> upval_local;
		unsigned int index;
		inline Value top(int i = 0);
		inline Value pop();
		inline void push(Value obj);
		inline Value getVar(unsigned int i);
		inline Value getUpVal(unsigned int i);
		inline void setVar(unsigned int i, Value obj);
		inline void setUpVal(unsigned int i, Value obj);
		inline Value getConstant(unsigned int i);
		void closeLoaclUpval();
		~Environment();
	};

	class StackVM final
	{
	public:
		const static unsigned int ENV_MAX = 255;

		StackVM() : env(nullptr)
		{
		}

		StackVM(const StackVM&) = delete;
		StackVM& operator=(const StackVM&) = delete;
		void execute();
		Environment* createEnvironment(CodePack *);
		void quitEnvironment();

		~StackVM()
		{
			if (env)
				delete env;
		}

	private:
		InstIter inst;
		Value* ptr;

		GC gc;

		Environment* env;
	};

}
