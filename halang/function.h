#pragma once
#include <vector>
#include <map>
#include "svm.h"
#include "type.h"
#include "string.h"
#include "object.h"
#include "upvalue.h"
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

		int findVarId(IString _name)
		{
			int _var_size = static_cast<int>(var_names.size());
			int _up_size = static_cast<int>(upvalue_names.size());

			for (int i = 0; i < _var_size; ++i)
				if (var_names[i].first == _name)
					return i;

			for (int i = 0; i < _up_size; ++i)
				if (upvalue_names[i].first == _name)
					return -2 - i;
			if (prev)
			{
				auto _prev_id = prev->findVarId(_name);
				auto _upvalue_id = upvalue_size++;
				upvalue_names.push_back(std::make_pair(_name, Type()));
				require_upvalues.push_back(_prev_id);
			}

			return -1;
		}

		friend class CodeGen;
		friend class StackVM;
		friend struct Environment;
	private:
		IString name;
		CodePack* prev;
		std::size_t param_size;
		std::size_t var_size;
		std::size_t upvalue_size;
		std::vector<Object> constant;
		std::vector<Instruction> instructions;
		std::vector<std::pair<IString, Type> > var_names;
		std::vector<std::pair<IString, Type> > upvalue_names;
		std::vector<int> require_upvalues;
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
		void close()
		{
			for (auto i = upvalues.begin(); i != upvalues.end(); ++i)
				(*i)->close();
		}
	private:
		CodePack* codepack;
		unsigned int paramsSize;
		std::vector<UpValue*> upvalues;
	};

};
