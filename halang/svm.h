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

		StackVM() 
		{ }

		StackVM(const StackVM&) = delete;
		StackVM& operator=(const StackVM&) = delete;

		void InitializeFunction(Function * );
		void Execute();

		void ChangeContext(ScriptContext*);

		~StackVM()
		{ }

	private:

		void InitalizeDefaultPrototype();

		InstIter inst;
		Value* ptr;

		GC gc;

		ScriptContext* sc;
	};

}
