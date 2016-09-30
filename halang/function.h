#pragma once
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include "svm.h"
#include "string.h"
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
			Object operator()(Isolate* iso, TNumber num)
			{
				return Object(num);
			}
		};

		template<>
		struct Constructor<TSmallInt>
		{
			Object operator()(Isolate* iso, TSmallInt si)
			{
				return Object(si);
			}
		};

		template<>
		struct Constructor<IString>
		{
			Object operator()(Isolate*)
			{
				return Object(IString());
			}

			Object operator()(Isolate*, const IString& _str)
			{
				return Object(IString(_str));
			}
		};

		template<>
		struct Constructor<Map>
		{
			Object operator()(Isolate* iso)
			{
				auto _map = iso->vm->make_gcobject<Map>();
			}
		};

		template<>
		struct Constructor<Array>
		{
			Object operator()(Isolate* iso)
			{
				auto arr = iso->vm->make_gcobject<Array>();
				return Object(arr, Object::TYPE::ARRAY);
			}

			Object operator()(Isolate* iso, Object arr)
			{
				Array* _arr = iso->vm->make_gcobject<Array>(*reinterpret_cast<Array*>(arr.value.gc));
				return Object(_arr, Object::TYPE::ARRAY);
			}

		};

		template<typename _Ty, typename... _Types> 
		inline Object New(_Types&&... _Args)
		{	
			Constructor<_Ty> con;
			return (con(this, std::forward<_Types>(_Args)...));
		}

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

		inline Object& operator[](std::size_t index)
		{
			return arguments[index];
		}

		inline Object& GetReturnObject()
		{
			return returnObject;
		}

		inline void SetReturnObject(Object obj)
		{
			returnObject = obj;
		}

		inline Isolate* GetIsolate()
		{
			return isolate;
		}

	private:

		Isolate* isolate;
		std::vector<Object> arguments;
		Object returnObject;

	};

	typedef std::function<void (const FunctionCallbackInfo&)> ExternFunction;

	class Function : public GCObject
	{
	public:

		Function(ExternFunction* fun, unsigned int _ps = 0) :
			paramsSize(_ps), externFunction(fun), isExtern(true)
		{}

		Function(CodePack* cp, unsigned int _ps = 0) :
			codepack(cp), paramsSize(_ps), isExtern(false)
		{}
		friend class CodeGen;
		friend struct Environment;
		friend class StackVM;

		inline Object getThisObject() const { return thisObject; }
		inline void setThisObject(Object _obj) { thisObject = _obj; }

		void close()
		{
			for (auto i = upvalues.begin(); i != upvalues.end(); ++i)
				(*i)->close();
		}

	private:

		bool isExtern;

		CodePack* codepack;
		ExternFunction *externFunction;
		Object thisObject;

		unsigned int paramsSize;
		std::vector<UpValue*> upvalues;

	};

};
