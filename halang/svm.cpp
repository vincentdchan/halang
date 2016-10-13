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

	void StackVM::InitializeFunction(Function* fun)
	{
		if (fun->isExtern)
			throw std::runtime_error("Main function can not be external.");
		auto new_sc = Context::GetGC()->New<ScriptContext>(fun);
		Context::InitializeDefaultPrototype();

		sc = new_sc;
		inst = fun->codepack->_instructions;
	}

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

	void StackVM::Execute()
	{
		for (;;)
		{
			InstIter current = inst++;
#ifdef DEBUG
			std::cout << "> " << static_cast<int>(current->getCode())
				<< " " << current->getParam() << std::endl;

#endif // DEBUG

			if (current->GetCode() == VM_CODE::STOP)
				break;
			Value t1;
			Function* func;
			switch (current->GetCode())
			{
			case VM_CODE::LOAD_V:
				PUSH(GET_VAR(current->GetParam()));
				break;
			case VM_CODE::LOAD_UPVAL:
			{
				auto _upval = GET_UPVAL(current->GetParam());
				PUSH(_upval->toValue());
				break;
			}
			case VM_CODE::LOAD_C:
			{
				func = sc->function;
				PUSH(func->codepack->_constants[current->GetParam()]);
				break;
			}
			case VM_CODE::STORE_V:
				SET_VAR(current->GetParam(), POP());
				break;
			case VM_CODE::STORE_UPVAL:
			{
				auto _id = current->GetParam();
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
			case VM_CODE::PUSH_NULL:
				PUSH(Value());
				break;
			case VM_CODE::PUSH_INT:
				PUSH(Value(current->GetParam()));
				break;
			case VM_CODE::PUSH_BOOL:
				PUSH(Value(current->GetParam() != 0));
				break;
			case VM_CODE::POP:
				POP();
				break;
			case VM_CODE::JMP:
				inst += current->GetParam() - 1;
				break;
			case VM_CODE::CLOSURE:
			{
				Value v1 = POP();
				func = reinterpret_cast<Function*>(v1.value.gc);

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
						_upval = sc->upvals[(-1 - cp->_require_upvalues[i])];

					func->upvalues.push_back(_upval);

				}
				PUSH(func->toValue());
				break;

			}
			case VM_CODE::CALL:
			{
				t1 = POP();
				Value This = POP();

				func = reinterpret_cast<Function*>(t1.value.gc);

				sc->saved_ptr = inst;

				auto new_sc = Context::GetGC()->New<ScriptContext>(func);

				auto params_size = current->GetParam();
				
				FunctionArgs * args = Context::GetGC()->New<FunctionArgs>(new_sc, params_size);

				for (int i = 0; i < params_size; ++i)
					args->Set(params_size - i - 1, POP());
				
				//	args->Push(sc->Pop());
				
				ChangeContext(new_sc);

				if (func->isExtern)
				{
					Value result =func->externFunction(This, *args);
					sc = sc->prev;
					sc->Push(result);
				}
				else
				{
					// load args
					for (unsigned int i = 0; i < args->GetLength(); ++i)
						new_sc->variables[i] = args->At(i);

					for (unsigned int i = 0;
						i < func->codepack->_upval_names_size; ++i)
						new_sc->upvals[i] = func->upvalues[i];

					inst = new_sc->function->codepack->_instructions;
				}

				break;

			}
			case VM_CODE::DOT:
			{
				Value vo1, vs2;
				vs2 = POP();
				vo1 = POP();

				if (vo1.isDict())
				{
					auto _dict = reinterpret_cast<Dict*>(vo1.value.gc);
					if (_dict->Exist(vs2))
					{
						PUSH(_dict->GetValue(vs2));
						break;
					}
				}

				auto proto = vo1.GetPrototype();
				auto target = proto->GetValue(vs2);;
				PUSH(vo1); // this
				PUSH(target);

				break;
			}
			case VM_CODE::RETURN:
				if (current->GetParam() != 0)
					sc->prev->Push(sc->Pop());
				sc = sc->prev;
				inst = sc->saved_ptr;
				break;
			case VM_CODE::IFNO:
				if (!POP())
					inst += current->GetParam() - 1;
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
