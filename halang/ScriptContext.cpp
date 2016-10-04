
#include "halang.h"
#include "ScriptContext.h"

namespace halang
{

		ScriptContext::ScriptContext(ScriptContext* _prev = nullptr, ScriptContext* _next = nullptr) :
			prev(_prev), next(_next),  
			constant_table(nullptr), constant_names(nullptr), constant_len(0),
			var_len(0), var_table(nullptr), var_names(nullptr), stack_base(nullptr),
			upval_table(nullptr), upval_names(nullptr), upval_len(0),
			stack_ptr(0), frame_ptr(0)
		{
			typeId = TypeId::ScriptContext;

			stack_base = Context::GetGc()->NewObjectPointerArray(STACK_SIZE);
		}

		void ScriptContext::initialize(size_type _const_size, size_type _var_size, size_type _upval_size)
		{
			constant_table = Context::GetGc()->NewObjectPointerArray(_const_size);
			constant_names = reinterpret_cast<String**>(Context::GetGc()->NewObjectPointerArray(_const_size));
			constant_len = _const_size;

			var_table = Context::GetGc()->NewObjectPointerArray(_var_size);
			var_names = reinterpret_cast<String**>(Context::GetGc()->NewObjectPointerArray(_var_size));
			var_len = _var_size;

			upval_table = reinterpret_cast<UpValue**>(Context::GetGc()->NewObjectPointerArray(_upval_size));
			upval_names = reinterpret_cast<String**>(Context::GetGc()->NewObjectPointerArray(_upval_size));
			upval_len = _upval_size;
		}

		void ScriptContext::Mark()
		{
			// stack must be marked
			GC::SetMark(stack_base);
			
			// if table is not null
			// mark the table itself and 
			// mark all the object in the table
			if (var_table != nullptr)
			{
				GC::SetMark(var_table);
				for (size_type i = 0; i < var_len; ++i)
				{
					if (var_table[i] != nullptr)
						var_table[i]->Mark();
				}
			}

			// mark the name itself
			if (var_names != nullptr)
			{
				GC::SetMark(var_names);
				for (size_type i = 0; i < var_len; ++i)
				{
					if (var_names[i] != nullptr)
						var_names[i]->Mark();
				}
			}

			// the same as upvalue
			if (upval_table != nullptr)
			{
				GC::SetMark(upval_table);
				for (size_type i = 0; i < upval_len; ++i)
				{
					if (upval_table[i] != nullptr)
						upval_table[i]->Mark();
				}
			}

			if (upval_names)
			{
				GC::SetMark(upval_names);
				for (size_type i = 0; i < upval_len; ++i)
				{
					if (upval_names[i] != nullptr)
						upval_names[i]->Mark();
				}
			}

			if (codePack != nullptr)
				codePack->Mark;

			if (prev != nullptr)
				prev->Mark();

		}

		void ScriptContext::Push(Object * obj)
		{
			if (stack_ptr >= STACK_SIZE)
				throw std::runtime_error("Script context stack overflow");
			stack_base[stack_ptr++] = obj;
		}

		Object* ScriptContext::Pop()
		{
			if (stack_ptr == 0)
				throw std::runtime_error("You can not pop on a empty script context stack.");
			return stack_base[--stack_ptr];
		}

		// if doesn't exist, return null object, not nullptr
		Object* ScriptContext::GetVariable(unsigned int index)
		{
			if (var_table[index] == nullptr)
				var_table[index] = Context::GetGc()->NewNull();
			return var_table[index];
		}

		UpValue* ScriptContext::GetUpValue(unsigned int index)
		{
			return upval_table[index];
		}

		void ScriptContext::SetVariable(unsigned int index, Object* obj)
		{
			if (index > var_len)
				throw std::runtime_error("index out of var range");
			var_table[index] = obj;
		}

		void ScriptContext::SetUpValue(unsigned int index, UpValue* upval)
		{
			if (index > upval_len)
				throw std::runtime_error("index out of upvalue range");
			upval_table[index] = upval;
		}

}
