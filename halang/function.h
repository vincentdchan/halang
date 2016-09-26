#pragma once
#include <vector>
#include <map>
#include "svm.h"
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
			prev(nullptr), param_size(0), isGlobal(false)
		{}

		/// <summary>
		/// Find the id in the var tables and
		/// return id of variable in table, return 0 if id is not found.
		/// </summary>
		/// <returns>return id of variable in table, return 0 if id is not found</returns>
		int findVarId(IString _name)
		{
			int _var_size = static_cast<int>(var_names.size());
			int _up_size = static_cast<int>(upvalue_names.size());

			for (int i = 0; i < _var_size; ++i)
				if (var_names[i] == _name)
					return i;

			for (int i = 0; i < _up_size; ++i)
				if (upvalue_names[i] == _name)
					return -2 - i;
			if (prev)
			{
				auto _prev_id = prev->findVarId(_name);
				addUpValue(name);
				require_upvalues.push_back(_prev_id);
			}

			return -1;
		}

		/// <summary>
		/// Add a variable name to the variable table and
		/// return the id of the new variable.
		/// </summary>
		/// <returns>return the id of the new variable</returns>
		inline std::size_t addVar(IString s)
		{
			auto id = var_names.size();
			var_names.push_back(s);
			return id;
		}

		/// <summary>
		/// Add a upvalue name to the upvalue table and
		/// return the id of the new upvalue.
		/// </summary>
		/// <returns>return the id of the new upvalue.</returns>
		inline std::size_t addUpValue(IString s)
		{
			auto id = upvalue_names.size();
			upvalue_names.push_back(s);
			return id;
		}

		inline std::size_t VarSize() const
		{
			return var_names.size();
		}

		inline std::size_t UpValueSize() const
		{
			return upvalue_names.size();
		}

		friend class CodeGen;
		friend class StackVM;
		friend struct Environment;
	private:
		IString name;
		CodePack* prev;
		std::size_t param_size;
		std::vector<Object> constant;
		std::vector<Instruction> instructions;
		std::vector<IString> var_names;
		std::vector<IString> upvalue_names;
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
