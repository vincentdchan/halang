#include "stdafx.h"
#include "svm.h"
#include "function.h"

namespace halang
{
	Environment::Environment(CodePack* cp) :
		prev(nullptr), sptr(nullptr), stack(nullptr)
		, codepack(cp), index(0)
	{
		sptr = stack = new Object[STACK_SIZE];
		variables = new Object[cp->var_size];
	}

	Object* Environment::top(int i) { return sptr - i - 1; }
	Object* Environment::pop() { return --sptr; }
	void Environment::push(Object&& obj) { *(sptr++) = obj; }
	Object* Environment::getVar(unsigned int i) { return variables + i; }
	void Environment::setVar(unsigned int i, Object&& obj) { variables[i] = obj; }
	Object Environment::getConstant(unsigned int i) { return codepack->constant[i]; }

	Environment::~Environment()
	{
		if (stack)
			delete[] stack;
		if (variables)
			delete[] variables;
	}

#define TOP(INDEX) env->top(INDEX) 
#define POP() env->pop()
#define PUSH(VAL) env->push(VAL)
#define GET_VAR(INDEX) env->getVar(INDEX)
#define SET_VAR(INDEX, VAL) env->setVar(INDEX, VAL)
#define GET_CON(INDEX) env->getConstant(INDEX)

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
			Object t1;
			Function* func;
			Environment *new_env;
			switch (current->getCode())
			{
			case VM_CODE::LOAD_V:
				PUSH(std::move(*GET_VAR(current->getParam())));
				break;
			case VM_CODE::LOAD_C:
				PUSH(GET_CON(current->getParam()));
				break;
			case VM_CODE::STORE_V:
				SET_VAR(current->getParam(), std::move(*POP()));
				break;
			case VM_CODE::PUSH_INT:
				PUSH(Object(current->getParam()));
				break;
			case VM_CODE::PUSH_BOOL:
				PUSH(Object(current->getParam() != 0));
				break;
			case VM_CODE::POP:
				POP();
				break;
			case VM_CODE::JMP:
				inst += current->getParam() - 1;
				break;
			case VM_CODE::CLOSURE:
				break;
			case VM_CODE::CALL:
				t1 = *POP();
				func = reinterpret_cast<Function*>(t1.value.gc);
				env->iter = inst;
				new_env = createEnvironment(func->codepack);
				for (int i = func->paramsSize - 1; i >= 0; --i)
					new_env->variables[i] = *new_env->prev->pop();
				break;
			case VM_CODE::RETURN:
				if (current->getParam() != 0)
				{
					env->prev->push(std::move(*env->pop()));
				}
				quitEnvironment();
				inst = env->iter;
				break;
			case VM_CODE::IFNO:
				if (!*(POP()))
					inst += current->getParam() - 1;
				break;
			case VM_CODE::NOT:
				PUSH(POP()->applyOperator(OperatorType::NOT));
			case VM_CODE::ADD:
				*TOP(1) = TOP(1)->applyOperator(OperatorType::ADD, *TOP(0));
				POP();
				break;
			case VM_CODE::SUB:
				*TOP(1) = TOP(1)->applyOperator(OperatorType::SUB, *TOP(0));
				POP();
				break;
			case VM_CODE::MUL:
				*TOP(1) = TOP(1)->applyOperator(OperatorType::MUL, *TOP(0));
				POP();
				break;
			case VM_CODE::DIV:
				*TOP(1) = TOP(1)->applyOperator(OperatorType::DIV, *TOP(0));
				POP();
				break;
			case VM_CODE::MOD:
				*TOP(1) = TOP(1)->applyOperator(OperatorType::MOD, *TOP(0));
				POP();
				break;
			case VM_CODE::GT:
				*TOP(1) = TOP(1)->applyOperator(OperatorType::GT, *TOP(0));
				POP();
				break;
			case VM_CODE::LT:
				*TOP(1) = TOP(1)->applyOperator(OperatorType::LT, *TOP(0));
				POP();
				break;
			case VM_CODE::GTEQ:
				*TOP(1) = TOP(1)->applyOperator(OperatorType::GTEQ, *TOP(0));
				POP();
				break;
			case VM_CODE::LTEQ:
				*TOP(1) = TOP(1)->applyOperator(OperatorType::LTEQ, *TOP(0));
				POP();
				break;
			case VM_CODE::EQ:
				*TOP(1) = TOP(1)->applyOperator(OperatorType::EQ, *TOP(0));
				POP();
				break;
			case VM_CODE::OUT:
				std::cout << *POP() << std::endl;
				break;
			}
		}
	}

	Environment* StackVM::createEnvironment(CodePack* cp)
	{
		if (env && env->index >= ENV_MAX)
			throw "Stack Full";

		auto new_env = new Environment(cp);
		if (!env)
			new_env->index = 0;
		else
			new_env->index = env->index + 1;

		new_env->prev = env;
		env = new_env;

		inst = new_env->codepack->instructions.begin();
		return new_env;
	}

	void StackVM::quitEnvironment()
	{
		auto current_env = env;
		env = current_env->prev;
		delete current_env;
	}

};
