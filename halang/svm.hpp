#pragma once
#ifdef DEBUG
#include <iostream>
#endif // DEBUG

#include <vector>
#include <cinttypes>
#include "svm_codes.h"
#include "object.h"

#define PRE(POINTER) ((POINTER) - 1)

namespace runtime
{
	namespace StackVM
	{


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
			template<typename InstIter>
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

		template<typename InstIter>
		class StackVM final
		{
		public:
			const static unsigned int VAR_STACK_SIZE = 255;
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
			~StackVM()
			{
				delete objStackBase_;
			}
		private:
			InstIter inst;
			Object* objStackBase_;
			Object* objStack;
			State* state;
		};

		template<typename InstIter>
		void StackVM<InstIter>::execute()
		{
			for (;;)
			{
				InstIter current = inst++;
#ifdef DEBUG
				std::cout << "> " << static_cast<int>(current->getCode()) 
					<< " " << current->getParam() << std::endl;

#endif // DEBUG

				if (current->getCode() == VM_CODE::STOP)
					break;
				switch (current->getCode())
				{
				case VM_CODE::LOAD_V:
					*(objStack++) = state->variables[current->getParam()];
					break;
				case VM_CODE::LOAD_C:
					*(objStack++) = state->constant[current->getParam()];
					break;
				case VM_CODE::STORE_V:
					state->variables[current->getParam()] = *(--objStack);
					break;
				case VM_CODE::PUSH_INT:
					(objStack++)->setSmallInt(current->getParam());
					break;
				case VM_CODE::PUSH_BOOL:
					(objStack++)->setBool(current->getParam() == 0 ? false : true);
					break;
				case VM_CODE::POP:
					--objStack;
					break;
				case VM_CODE::JMP:
					inst += current->getParam() - 1;
					break;
				case VM_CODE::IFNO:
					if (!*(--objStack))
						inst += current->getParam() - 1;
					break;
				case VM_CODE::ADD:
					--objStack;
					*PRE(objStack) = PRE(objStack)->applyOperator(OperatorType::ADD, *objStack);
					break;
				case VM_CODE::SUB:
					--objStack;
					*PRE(objStack) = PRE(objStack)->applyOperator(OperatorType::SUB, *objStack);
					break;
				case VM_CODE::MUL:
					--objStack;
					*PRE(objStack) = PRE(objStack)->applyOperator(OperatorType::MUL, *objStack);
					break;
				case VM_CODE::DIV:
					--objStack;
					*PRE(objStack) = PRE(objStack)->applyOperator(OperatorType::DIV, *objStack);
					break;
				case VM_CODE::MOD:
					--objStack;
					*PRE(objStack) = PRE(objStack)->applyOperator(OperatorType::MOD, *objStack);
					break;
				case VM_CODE::GT:
					--objStack;
					*PRE(objStack) = PRE(objStack)->applyOperator(OperatorType::GT, *objStack);
					break;
				case VM_CODE::LT:
					--objStack;
					*PRE(objStack) = PRE(objStack)->applyOperator(OperatorType::LT, *objStack);
					break;
				case VM_CODE::GTEQ:
					--objStack;
					*PRE(objStack) = PRE(objStack)->applyOperator(OperatorType::GTEQ, *objStack);
					break;
				case VM_CODE::LTEQ:
					--objStack;
					*PRE(objStack) = PRE(objStack)->applyOperator(OperatorType::LTEQ, *objStack);
					break;
				case VM_CODE::EQ:
					--objStack;
					*PRE(objStack) = PRE(objStack)->applyOperator(OperatorType::EQ, *objStack);
					break;
				case VM_CODE::OUT:
					std::cout << *(--objStack) << std::endl;
					break;
				}
			}
		}

	}

}
