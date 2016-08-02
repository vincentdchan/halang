#pragma once
#include <vector>
#include <map>
#include "svm.h"
#include "object.h"
#include "svm_codes.h"

namespace halang
{

	//
	// A codepack is a package of codes
	//
	// Including :
	//		instruction bytes
	//		variables names to the id
	//
	class CodePack : public GCObject
	{
	public:
		CodePack() :
			prev(nullptr), param_size(0), var_size(0), isGlobal(false)
		{}

		int findVarId(const std::string& _name)
		{
			int _size = static_cast<int>(var_names.size());
			for (int i = 0; i < _size; ++i)
				if (var_names[i] == _name)
					return i;
			return -1;
		}

		friend class CodeGen;
		friend class StackVM;
		friend struct Environment;
	private:
		CodePack* prev;
		std::size_t param_size;
		std::size_t var_size;
		std::vector<Object> constant;
		std::vector<Instruction> instructions;
		std::vector<std::string> var_names;
		bool isGlobal;
	};

	class Function : public GCObject
	{
	public:
		Function(CodePack* cp, unsigned int _ps = 0) :
			codepack(cp), paramsSize(_ps)
		{}
		friend class CodeGen;
		friend struct Environment;
		friend class StackVM;
	private:
		CodePack* codepack;
		unsigned int paramsSize;
	};

};
