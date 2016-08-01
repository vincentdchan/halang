#pragma once
#include <iostream>
#include <vector>
#include <cinttypes>
#include <utility>
#include <map>
#include "svm_codes.h"
#include "object.h"
#include "upvalue.h"

#define PRE(POINTER) ((POINTER) - 1)

namespace halang
{
	class GlobalState;
	class FunctionState;

	class GlobalState
	{
		std::size_t var_size;
		std::map<std::size_t, std::string> var_names;
		Object* variables;

		GCObject* gc_objects;

		void add_gc(GCObject* _obj)
		{
			// make linked list
			_obj->next = this->gc_objects;
			this->gc_objects = _obj;
		}

	public:
		friend class StackVM;
		~GlobalState()
		{
			delete[] variables;
		}

		//fuck all
	};

	class FunctionState
	{
		GlobalState* globalstate;
		FunctionState* parent;
		std::size_t var_size;
		std::size_t upvalues_size;
		Object* stack;

		Object* objStackBase_;
		Object* objStack;

		Object* variables;
		Object* upvalues;

		std::map<std::size_t, std::string> var_names;
		std::vector<std::size_t> up_variables; // when the funciton finish close the upvalue

	public:
		friend class StackVM;
		~FunctionState()
		{
			delete[] objStackBase_;
			delete[] variables;
			delete[] upvalues;
		}
	};

	class State final
	{
	public:
		State(std::size_t _var_size, const std::vector<Object>& _const) :
			var_size(_var_size), constant(_const)
		{
			variables = new Object[_var_size];
		}
		State(const State&) = delete;
		State& operator=(const State&) = delete;
		~State()
		{
			delete variables;
		}
		friend class StackVM;
	private:
		std::size_t var_size;
		Object* variables;
		std::vector<Object> constant;
	};

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

	class StackVM final
	{
	public:
		const static unsigned int VAR_STACK_SIZE = 255;
		typedef std::vector<Instruction>::iterator InstIter;

		StackVM()
		{}

		StackVM(InstIter _inst) : inst(_inst)
		{
			objStack = objStackBase_ = new Object[VAR_STACK_SIZE];
		}
		StackVM(StackVM&& _svm)
		{
			this->objStackBase_ = _svm.objStackBase_;
			this->objStack = _svm.objStack;
			inst = _svm.inst;

			this->objStack = nullptr;
			this->objStackBase_ = nullptr;
		}
		StackVM(const StackVM&) = delete;
		StackVM& operator=(const StackVM&) = delete;
		void setState(State* st) { state = st; }
		void execute();

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
			delete objStackBase_;
		}
	private:
		InstIter inst;
		Object* objStackBase_;
		Object* objStack;
		GCObject* gcobj_list;
		State* state;
	};

}
