#pragma once
#include <iostream>
#include <vector>
#include <cinttypes>
#include <utility>
#include <map>
#include "GC.h"
#include "svm_codes.h"
#include "ScriptContext.h"

#define PRE(POINTER) ((POINTER) - 1)

namespace halang
{
	class CodePack;

	typedef Instruction* InstIter;

	class StackVM final
	{
	public:

		friend class GC;

		class Executor;

		StackVM();

		StackVM(const StackVM&) = delete;
		StackVM& operator=(const StackVM&) = delete;

		void InitializeFunction(Function * );


		void CallFunction(Function* function, Value self, FunctionArgs * args = nullptr);

		~StackVM();

	private:

		InstIter inst;
		Value* ptr;

		GC gc;

	};

	class StackVM::Executor 
	{
	public:

		Executor(Value _self, Function *, FunctionArgs * args = nullptr);

		void LoadArguments(FunctionArgs *);
		void Execute();
		Value ReturnValue();
		~Executor();

	private:

		Value self;
		ScriptContext* sc;
		Function* fun;
		FunctionArgs * args;
		InstIter inst;
		Value returnValue;

	};

}
