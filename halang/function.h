#pragma once
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include "svm.h"
#include "String.h"
#include "Array.h"
#include "object.h"
#include "upvalue.h"
#include "svm_codes.h"

namespace halang
{

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

		friend class VM;
		friend class CodeGen;
		friend class StackVM;
		friend class Function;

		typedef unsigned int size_type;

	protected:
		CodePack() :
			prev(nullptr), param_size(0)
		{}

		/// <summary>
		/// Find the id in the var tables and
		/// return id of variable in table, return 0 if id is not found.
		/// </summary>
		/// <returns>return id of variable in table, return 0 if id is not found</returns>
		/*
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
		*/

	private:

		CodePack* prev;
		String* name;
		std::size_t param_size;

		Value* _constants;
		size_type _const_size;

		Instruction* _instructions;
		size_type _instructions_size;

		int* _require_upvalues;
		size_type _require_upvales_size;

		// GC Object

		String* _var_names;
		size_type _var_names_size;

		String* _upval_names;
		size_type _upval_names_size;

	public:

		virtual Value toValue() override
		{
			return Value(this, TypeId::CodePack);
		}

		~CodePack()
		{
			delete[] _constants;
			delete[] _instructions;
			delete[] _require_upvalues;
		}

	};

	class Isolate
	{
	public:
		Isolate() :
			codepack(nullptr)
		{}

		template<typename _Type>
		struct Constructor : std::false_type
		{};

		template<>
		struct Constructor<TNumber>
		{
			Value operator()(Isolate* iso, TNumber num)
			{
				return Value(num);
			}
		};

		template<>
		struct Constructor<TSmallInt>
		{
			Value operator()(Isolate* iso, TSmallInt si)
			{
				return Value(si);
			}
		};

		template<>
		struct Constructor<String>
		{
			Value operator()(Isolate*)
			{
				return Context::GetGC()->New<String>()->toValue();
			}

			Value operator()(Isolate*, String* _str)
			{
				return _str->toValue();
			}
		};

		template<>
		struct Constructor<Array>
		{
			Value operator()(Isolate* iso)
			{
				auto arr = Context::GetGC()->New<Array>();
				return arr->toValue();;
			}

		};

		inline StackVM* GetVM() const { return vm; }

	private:
		CodePack* codepack;
		StackVM* vm;
	};

	class FunctionCallbackInfo : GCObject
	{
	public:

		FunctionCallbackInfo(Isolate* iso) :
			isolate(iso)
		{}

		inline Value& operator[](std::size_t index)
		{
			return arguments[index];
		}

		inline Value& GetReturnObject()
		{
			return returnObject;
		}

		inline void SetReturnObject(Value obj)
		{
			returnObject = obj;
		}

		inline Isolate* GetIsolate()
		{
			return isolate;
		}

	private:

		Isolate* isolate;
		std::vector<Value> arguments;
		Value returnObject;

	};

	typedef std::function<void (const FunctionCallbackInfo&)> ExternFunction;

	class Function : public GCObject
	{
	public:

		friend class GC;
		friend class StackVM;
		friend struct Environment;

		typedef unsigned int size_type;

	protected:

		Function(ExternFunction fun, unsigned int _ps = 0) :
			paramsSize(_ps), externFunction(fun), isExtern(true)
		{}

		Function(CodePack* cp, unsigned int _ps = 0) :
			codepack(cp), paramsSize(_ps), isExtern(false)
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

		size_type paramsSize;
		std::vector<UpValue*> upvalues;

		inline void SetThisObject(Value _obj) { thisOne = _obj; }

	public:

		inline Value GetThis() const { return thisOne; }

		virtual ~Function()
		{

		}

	};

};
