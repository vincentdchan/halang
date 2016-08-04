#pragma once
#include <iostream>
#include <vector>
#include <cinttypes>
#include <utility>
#include <map>
#include "string_pool.h"
#include "svm_codes.h"
#include "object.h"
#include "upvalue.h"

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
		Object* stack;
		Object* sptr;

		InstIter iter;
		Object* variables;
		CodePack* codepack;
		unsigned int index;
		inline Object* top(int i = 0);
		inline Object* pop();
		inline void push(Object&& obj);
		inline Object* getVar(unsigned int i);
		inline void setVar(unsigned int i, Object&& obj);
		inline Object getConstant(unsigned int i);
		~Environment();
	};

	class StackVM final
	{
	public:
		const static unsigned int ENV_MAX = 255;

		StackVM() : env(nullptr)
		{
			string_pool = new StringPool();
		}

		/*
		StackVM(StackVM&& _svm)
		{
			this->objStackBase_ = _svm.objStackBase_;
			this->objStack = _svm.objStack;
			inst = _svm.inst;

			this->objStack = nullptr;
			this->objStackBase_ = nullptr;
		}
		*/
		StackVM(const StackVM&) = delete;
		StackVM& operator=(const StackVM&) = delete;
		void execute();
		Environment* createEnvironment(CodePack *);
		void quitEnvironment();

		template<typename _Type, typename... _AT>
		_Type* make_gcobject(_AT&&... args)
		{
			_Type* obj = new _Type(std::forward<_AT>(args)...);
			obj->next = gcobj_list;
			gcobj_list = obj;
			return obj;
		}

		~StackVM()
		{
			if (env)
				delete env;
			delete string_pool;

			GCObject* optr = gcobj_list;
			while (optr)
			{
				auto next = optr->next;
				delete optr;
				optr = next;
			}
		}

		StringPool* string_pool;
	private:
		InstIter inst;
		Object* ptr;
		GCObject* gcobj_list;

		Environment* env;
		// State* state;
	};

}
