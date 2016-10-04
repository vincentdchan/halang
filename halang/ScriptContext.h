#pragma once
#include <cinttypes>
#include "halang.h"
#include "object.h"
#include "context.h"
#include "GC.h"
#include "function.h"

namespace halang
{

	class ScriptContext : public Object
	{
	public:

		typedef std::uint32_t size_type;

		friend class StackVM;
		friend class GC;

		static const size_type STACK_SIZE = 256;

	private:

		Object** constant_table;
		String** constant_names;
		size_type constant_len;

		Object** var_table;
		String** var_names;
		size_type var_len;

		UpValue** upval_table;
		String** upval_names;
		size_type upval_len;

		Object** stack_base;
		unsigned int stack_ptr;

		ScriptContext* prev;
		ScriptContext* next;

		CodePack* codePack;
		unsigned int frame_ptr;

	protected:

		ScriptContext(ScriptContext* _prev = nullptr, ScriptContext* _next = nullptr);
		void initialize(size_type const_size, size_type _var_size, size_type _upval_size);

		void SetVariable(unsigned int, Object*);
		void SetUpValue(unsigned int, UpValue*);

	public:
		friend class StackVM;
		friend class GC;

		void Push(Object*);
		Object* Pop();
		Object* GetConstant(unsigned int);
		Object* GetVariable(unsigned int);
		UpValue* GetUpValue(unsigned int);

		virtual void Mark() override;

	};

}
