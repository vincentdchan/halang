#include "svm.h"
#include "function.h"
#include "upvalue.h"
#include "string.h"
#include "GC.h"
#include "Dict.h"
#include "context.h"

#define TOP(INDEX) sc->Top(INDEX) 
#define POP() sc->Pop()
#define PUSH(VAL) sc->Push(VAL)
#define GET_VAR(INDEX) sc->GetVariable(INDEX)
#define GET_UPVAL(INDEX) sc->GetUpValue(INDEX)
#define SET_VAR(INDEX, VAL) sc->SetVariable(INDEX, VAL)
#define SET_UPVAL(INDEX, VAL) sc->SetUpValue(INDEX, VAL)

namespace halang
{

	StackVM::StackVM()
	{
		Context::vm = this;
		Context::runningContexts = new std::vector<ScriptContext*>();

		Context::InitializeDefaultPrototype();
	}

	StackVM::~StackVM()
	{
		delete Context::runningContexts;
		Context::runningContexts = nullptr;
	}

	void StackVM::InitializeFunction(Function* fun)
	{
	}


	Value StackVM::CallFunction(Function* _fun, Value _self, FunctionArgs * args)
	{

		if (args == nullptr)
			args = Context::GetGC()->New<FunctionArgs>();
		Executor executor(_self, _fun, args);
		executor.Execute();

		return executor.ReturnValue();
	}

	StackVM::Executor::Executor(Value _self, Function * _fn,
		FunctionArgs * _args) :
		self(_self), fun(_fn), args(_args)
	{
		sc = Context::GetGC()->New<ScriptContext>(_fn);
		Context::GetRunningContexts()->push_back(sc);
	}

	StackVM::Executor::~Executor()
	{
		Context::GetRunningContexts()->pop_back();
	}

	void StackVM::Executor::LoadArguments(FunctionArgs * _args)
	{
		args = _args;
	}

	void StackVM::Executor::Execute()
	{
		if (fun->isExtern)
			returnValue = fun->externFunction(self, *args);
		else
		{
			// load args
			for (unsigned int i = 0; i < args->GetLength(); ++i)
				sc->variables[i] = args->At(i);

			inst = sc->function->codepack->_instructions;

			for (;;)
			{
				InstIter current = inst++;

				Value t1;
				Function* func;

				switch (current->GetCode())
				{
				case VM_CODE::STOP:
					goto OUT_LOOP;
					break;
				case VM_CODE::LOAD_V:
					PUSH(GET_VAR(current->GetParam()));
					break;
				case VM_CODE::LOAD_UPVAL:
				{
					auto _upval = GET_UPVAL(current->GetParam());
					PUSH(_upval->GetVal());
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
					for (unsigned int i = 0; i < cp->_require_upvalues_size; ++i)
					{

						if (cp->_require_upvalues[i] >= 0)
						{
							_upval = Context::GetGC()->New<UpValue>(sc->variables + cp->_require_upvalues[i]);
							sc->host_upvals.push_back(_upval);
						}
						else
							_upval = sc->function->upvalues[(-1 - cp->_require_upvalues[i])];

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

					auto params_size = current->GetParam();

					FunctionArgs * args = Context::GetGC()->New<FunctionArgs>(params_size);

					for (int i = 0; i < params_size; ++i)
						args->Set(params_size - i - 1, POP());

					Executor executor(This, func, args);

					executor.Execute();
					sc->Push(executor.ReturnValue());

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
					Value vfun;

					bool ok = proto->TryGetValue(vs2, vfun);
					if (ok)
					{
						PUSH(vo1); // this
						PUSH(vfun);
					}
					else
						throw std::runtime_error("This object does not contain that property.");

					break;
				}
				case VM_CODE::RETURN:
					if (current->GetParam() != 0)
						returnValue = sc->Pop();

					sc->CloseAllUpValue();

					goto OUT_LOOP;
					break;
				case VM_CODE::IFNO:
					if (!POP())
						inst += current->GetParam() - 1;
					break;
				case VM_CODE::OUT:
					break;
				}

				Context::GetGC()->CheckAndGC();
			}
		OUT_LOOP:
			;
		}
	}

	Value StackVM::Executor::ReturnValue()
	{
		return returnValue;
	}

}
