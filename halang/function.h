#pragma once
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include "String.h"
#include "Array.h"
#include "object.h"
#include "upvalue.h"
#include "svm_codes.h"

namespace halang
{

	class ConstantTable : public GCObject
	{
	private:


	};

	/// <summary>
	/// A codepack is a package of codes
	///
	/// Including :
	///		instruction bytes
	///		variables names to the id
	///
	/// CodePack store the envrionment.
	///
	/// </summary>
	class CodePack : public GCObject
	{
	public:

		friend class GC;
		friend class CodeGen;
		friend class StackVM;
		friend class Function;
		friend class ScriptContext;

		typedef unsigned int size_type;

	protected:

		CodePack() :
			prev(nullptr), param_size(0),
			_var_names(nullptr), _upval_names(nullptr),
			_var_names_size(0), _upval_names_size(0),
			_require_upvalues(nullptr), _require_upvalues_size(0)
		{}

	private:

		CodePack* prev;
		String* name;
		std::size_t param_size;

		Value* _constants;
		size_type _const_size;

		Instruction* _instructions;
		size_type _instructions_size;

		int* _require_upvalues;
		size_type _require_upvalues_size;

		// GC Object

		String** _var_names;
		size_type _var_names_size;

		String** _upval_names;
		size_type _upval_names_size;

	public:

		inline void GenerateVarNamesArray(size_type _size)
		{
			_var_names_size = _size;
			_var_names = _size > 0 ? new String*[_size] : nullptr;
		}

		inline void SetVarName(size_type index, String* name)
		{
			if (index >= _var_names_size)
				throw std::runtime_error("index out of range");
			_var_names[index] = name;
		}

		inline void GenerateUpValNamesArray(size_type _size)
		{
			_upval_names_size = _size;
			_upval_names = _size > 0 ? new String*[_size] : nullptr;
		}

		inline void SetUpValName(size_type index, String* name)
		{
			if (index >= _upval_names_size)
				throw std::runtime_error("index out of range");
			_upval_names[index] = name;
		}

		virtual void Mark() override;

		virtual Dict* GetPrototype() override
		{
			return nullptr;
		}

		virtual Value toValue() override
		{
			return Value(this, TypeId::CodePack);
		}

		virtual ~CodePack()
		{
			if (_var_names != nullptr)
				delete[] _var_names;
			if (_upval_names != nullptr)
				delete[] _upval_names;
			delete[] _constants;
			delete[] _instructions;
			delete[] _require_upvalues;
		}

	};

	class FunctionArgs : public Array
	{
	public:

		friend class GC;
		friend class StackVM;

	protected:

		FunctionArgs()
		{
		}

		FunctionArgs(unsigned int i):
			Array(i)
		{
		}

	public:

		virtual void Mark() override;

	};

	typedef std::function<Value (Value, FunctionArgs& )> ExternFunction;

	class Function : public GCObject
	{
	public:

		friend class GC;
		friend class StackVM;
		friend class CodeGen;
		friend class ScriptContext;

		typedef unsigned int size_type;

		static std::string ToString(Function *);

	protected:

		Function(ExternFunction fun) :
			externFunction(fun), isExtern(true)
		{}

		Function(CodePack* cp) :
			codepack(cp), isExtern(false)
		{}

		void Close()
		{
			for (auto i = upvalues.begin(); i != upvalues.end(); ++i)
				(*i)->close();
		}

	private:

		bool isExtern;

		union 
		{
			CodePack* codepack;
			ExternFunction externFunction;
		};

		String * name;
		Value thisOne;

		std::vector<UpValue*> upvalues;

		inline void SetThisObject(Value _obj) { thisOne = _obj; }
		virtual Dict* GetPrototype() override { return nullptr; }

	public:

		virtual void Mark() override;

		virtual Value toValue() override { return Value(this, TypeId::Function); }

		inline Value GetThis() const { return thisOne; }

		virtual ~Function()
		{
		}

	};

};
