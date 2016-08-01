#include "stdafx.h"
#include "svm.h"

namespace halang
{

	void StackVM::execute()
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
			case VM_CODE::NOT:
				*PRE(objStack) = PRE(objStack)->applyOperator(OperatorType::NOT);
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

};
