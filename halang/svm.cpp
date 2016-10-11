#include "stdafx.h"
#include "svm.h"
#include "function.h"
#include "upvalue.h"
#include "string.h"
#include "GC.h"
#include "Dict.h"
#include "context.h"

namespace halang
{
	void StackVM::ChangeContext(ScriptContext* new_sc)
	{
		if (!new_sc->function->isExtern)
			sc->saved_ptr = inst;
		new_sc->prev = sc;
		sc = new_sc;
	}

#define TOP(INDEX) sc->Top(INDEX) 
#define POP() sc->Pop()
#define PUSH(VAL) sc->Push(VAL)
#define GET_VAR(INDEX) sc->GetVariable(INDEX)
#define GET_UPVAL(INDEX) sc->GetUpValue(INDEX)
#define SET_VAR(INDEX, VAL) sc->SetVariable(INDEX, VAL)
#define SET_UPVAL(INDEX, VAL) sc->SetUpValue(INDEX, VAL)

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
			switch (current->getCode())
			{
			case VM_CODE::LOAD_V:
				PUSH(GET_VAR(current->getParam()));
				break;
			case VM_CODE::LOAD_UPVAL:
			{
				auto _upval = GET_UPVAL(current->getParam());
				PUSH(_upval->toValue());
				break;
			}
			case VM_CODE::LOAD_C:
			{
				func = sc->function;
				PUSH(func->codepack->_constants[current->getParam()]);
				break;
			}
			case VM_CODE::STORE_V:
				SET_VAR(current->getParam(), POP());
				break;
			case VM_CODE::STORE_UPVAL:
			{
				auto _id = current->getParam();
				auto _upval = GET_UPVAL(_id);
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
			{
				Value v1 = POP();
				func = reinterpret_cast<Function*>(t1.value.gc);

				CodePack* cp = func->codepack;
				UpValue* _upval = nullptr;
				for (unsigned int i = 0; i < cp->_require_upvales_size; ++i)
				{

					if (cp->_require_upvalues[i] >= 0)
					{
						_upval = Context::GetGC()->New<UpValue>(sc->variables + cp->_require_upvalues[i]);
						sc->PushUpValue(_upval);
					}
					else
						_upval = Context::GetGC()->New<UpValue>(sc->upvals + (-1 - cp->_require_upvalues[i]));

					func->upvalues.push_back(_upval);

				}
				PUSH(func->toValue());
				break;

			}
			case VM_CODE::CALL:
			{

				t1 = POP();
				func = reinterpret_cast<Function*>(t1.value.gc);

				sc->saved_ptr = inst;

				auto new_sc = Context::GetGC()->New<ScriptContext>(func);


				FunctionArgs * args = Context::GetGC()->New<FunctionArgs>(new_sc);

				for (int i = 0; i < func->paramsSize; ++i)
					args->Push(sc->Pop());

				for (unsigned int i = 0;
					i < func->codepack->_upval_names_size; ++i)
					new_sc->upvals[i] = func->upvalues[i];
				
				ChangeContext(new_sc);

				if (func->isExtern)
				{
					func->externFunction(Value(), args);
					sc = sc->prev;
				}
				else
				{
					inst = new_sc->function->codepack->_instructions;
				}

				break;

			}
			case VM_CODE::RETURN:
				if (current->getParam() != 0)
					sc->prev->Push(sc->Pop());
				sc = sc->prev;
				inst = sc->saved_ptr;
				break;
			case VM_CODE::IFNO:
				if (!POP())
					inst += current->getParam() - 1;
				break;
			case VM_CODE::NOT:
				PUSH(String::FromCharArray("__not__")->toValue());
				break;
			case VM_CODE::ADD:
				PUSH(String::FromCharArray("__add__")->toValue());
				break;
			case VM_CODE::SUB:
				PUSH(String::FromCharArray("__sub__")->toValue());
				break;
			case VM_CODE::MUL:
				PUSH(String::FromCharArray("__mul__")->toValue());
				break;
			case VM_CODE::DIV:
				PUSH(String::FromCharArray("__mul__")->toValue());
				break;
			case VM_CODE::MOD:
				PUSH(String::FromCharArray("__mod__")->toValue());
				break;
			case VM_CODE::GT:
				PUSH(String::FromCharArray("__gt__")->toValue());
				break;
			case VM_CODE::LT:
				PUSH(String::FromCharArray("__lt__")->toValue());
				break;
			case VM_CODE::GTEQ:
				PUSH(String::FromCharArray("__gteq__")->toValue());
				break;
			case VM_CODE::LTEQ:
				PUSH(String::FromCharArray("__gteq__")->toValue());
				break;
			case VM_CODE::EQ:
				PUSH(String::FromCharArray("__eq__")->toValue());
				break;
			case VM_CODE::OUT:
				break;
			}
		}
	}

};
