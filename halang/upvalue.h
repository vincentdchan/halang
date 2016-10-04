#pragma once
#include "halang.h"
#include "object.h"
#include "context.h"

namespace halang
{

	/// <summary>
	/// UpValue is a magic class.
	///
	/// Before the upvalue is closed, upvalue store the pointer 
	/// of the object.
	/// After the upvalue is closed, upvalue construct a new
	/// Object and own it.
	/// </summary>
	class UpValue : public Object
	{
	protected:
		union
		{
			Object *value;
			Object** refer;
		};

		bool _closed;

		UpValue()
		{
			typeId = TypeId::UpValue;
		}

	public:
		UpValue(Object** _re = nullptr) : refer(_re), _closed(false)
		{}

		inline Object* getVal() const
		{
			if (!_closed)
				return *refer;
			else
				return value;
		}

		inline void setVal(Object* _obj)
		{
			if (!_closed)
				*refer = _obj;
			else
				value = _obj;
		}

		void close()
		{
			if (!_closed) 
			{
				value = *refer;
				_closed = true;
			}
		}

		inline bool closed() const { return _closed; }

		virtual void Mark() override
		{
			if (!_closed)
				(*refer)->Mark();
			else
				value->Mark();
		}

	};

}
