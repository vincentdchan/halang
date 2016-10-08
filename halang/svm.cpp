#include "stdafx.h"
#include "svm.h"
#include "function.h"
#include "upvalue.h"
#include "string.h"
#include "GC.h"
#include "Dict.h"

namespace halang
{
	Environment::Environment(CodePack* cp) :
		prev(nullptr), sptr(nullptr), stack(nullptr)
		, codepack(cp), index(0)
	{
		sptr = stack = new Value[STACK_SIZE];
		variables = new Value[cp->VarSize()];
		upvalues = new Value[cp->UpValueSize()];
	}

	Value Environment::top(int i) { return *(sptr - i - 1); }
	Value Environment::pop() { return *(--sptr); }
	void Environment::push(Value obj) { *(sptr++) = obj; }
	Value Environment::getVar(unsigned int i) { return variables[i]; }
	Value Environment::getUpVal(unsigned int i) { return upvalues[i]; }
	void Environment::setVar(unsigned int i, Value obj) { variables[i] = obj; }
	void Environment::setUpVal(unsigned int i, Value obj) { upvalues[i] = obj; }
	Value Environment::getConstant(unsigned int i) { return codepack->constant[i]; }

	void Environment::closeLoaclUpval()
	{
		for (auto i = upval_local.begin(); i != upval_local.end(); ++i)
			(*i)->close();
	}

	Environment::~Environment()
	{
		closeLoaclUpval();
		if (stack)
			delete[] stack;
		if (variables)
			delete[] variables;
		if (upvalues)
			delete[] upvalues;
	}

#define TOP(INDEX) env->top(INDEX) 
#define POP() env->pop()
#define PUSH(VAL) env->push(VAL)
#define GET_VAR(INDEX) env->getVar(INDEX)
#define GET_UPVAL(INDEX) env->getUpVal(INDEX)
#define SET_VAR(INDEX, VAL) env->setVar(INDEX, VAL)
#define SET_UPVAL(INDEX, VAL) env->setUpVal(INDEX, VAL)
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
			Value t1;
			Function* func;
			Environment *new_env;
			switch (current->getCode())
			{
			case VM_CODE::LOAD_V:
				PUSH(GET_VAR(current->getParam()));
				break;
			case VM_CODE::LOAD_UPVAL:
			{
				auto _upval = reinterpret_cast<UpValue*>(GET_UPVAL(current->getParam()).value.gc);
				PUSH(_upval->toValue());
				break;
			}
			case VM_CODE::LOAD_C:
				PUSH(GET_CON(current->getParam()));
				break;
			case VM_CODE::STORE_V:
				SET_VAR(current->getParam(), POP());
				break;
			case VM_CODE::STORE_UPVAL:
			{
				auto _id = current->getParam();
				auto _upval = reinterpret_cast<UpValue*>(GET_UPVAL(_id).value.gc);
				_upval->SetVal(POP());
				break;
			}
			case VM_CODE::SET_VAL:
			{
				auto value = POP();
				auto key = POP();
				auto _dict = reinterpret_cast<Dict*>((POP()).value.gc);
				_dict->SetValue(key, value);
				PUSH(_dict->toValue());
				break;
			}
			case VM_CODE::GET_VAL:
			{
				auto key = POP();
				auto dict = reinterpret_cast<Dict*>((POP()).value.gc);
				PUSH(dict->GetValue(key));
				break;
			}
			case VM_CODE::PUSH_INT:
				PUSH(Value(current->getParam()));
				break;
			case VM_CODE::PUSH_BOOL:
				PUSH(Value(current->getParam() != 0));
				break;
			case VM_CODE::POP:
				POP();
				break;
			case VM_CODE::JMP:
				inst += current->getParam() - 1;
				break;
			case VM_CODE::CLOSURE:
				t1 = POP();
				func = reinterpret_cast<Function*>(t1.value.gc);
				do {
					CodePack* cp = func->codepack;
					UpValue * _upval = nullptr;
					for (auto i = cp->require_upvalues.begin();
						i != cp->require_upvalues.end(); ++i)
					{
						if (*i >= 0)
						{
							_upval = gc.New<UpValue>(env->variables + *i);
							env->upval_local.push_back(_upval);
						}
						else
							_upval = gc.New<UpValue>(env->upvalues + (-1 - *i));
						func->upvalues.push_back(_upval);
					}
				} while (0);
				PUSH(Value(func, TypeId::Function));
				break;
			case VM_CODE::CALL:
				t1 = POP();
				func = reinterpret_cast<Function*>(t1.value.gc);
				env->iter = inst;
				new_env = createEnvironment(func->codepack);

				// copy parameters
				for (int i = func->paramsSize - 1; i >= 0; --i)
					new_env->variables[i] = new_env->prev->pop();
				// copy upvalues
				for (unsigned int i = 0; i < func->codepack->UpValueSize(); ++i)
				{
					new_env->upvalues[i] = Value(func->upvalues[i], TypeId::UpValue);
				}
				break;
			case VM_CODE::RETURN:
				if (current->getParam() != 0)
				{
					env->prev->push(env->pop());
				}
				quitEnvironment();
				inst = env->iter;
				break;
			case VM_CODE::IFNO:
				if (!POP())
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
