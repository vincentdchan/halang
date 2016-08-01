#pragma once
#include <vector>
#include <map>
#include "svm.hpp"
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
		friend class CodeGen;
	private:
		CodePack* prev;
		std::size_t param_size;
		std::size_t var_size;
		std::vector<Object> constant;
		std::vector<Instruction> instructions;
		std::map<std::size_t, std::string> var_names;
		bool isGlobal;
	};

	class Function : public GCObject
	{
	public:
		Function(CodePack* cp, unsigned int _ps = 0) :
			codepack(cp), paramsSize(_ps)
		{}
	private:
		CodePack* codepack;
		unsigned int paramsSize;
	};

};
