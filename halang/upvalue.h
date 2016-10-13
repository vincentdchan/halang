#pragma once
#include "object.h"

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
	class UpValue : public GCObject
	{
	protected:

		UpValue(Value* _re = nullptr): 
			value(_re), _closed(false)
		{}

	public:

		friend class GC;
		friend class StackVM;

		virtual void Mark() override
		{
			if (value->isGCObject())
				value->value.gc->Mark();
		}

		virtual ~UpValue() 
		{
			if (_closed)
				delete value;
		}

		Value GetVal() const
		{
			return *value;
		}

		void SetVal(Value _v)
		{
			*value = _v;
		}

		void close()
		{
			if (!_closed) 
			{
				value = new Value(*value);
				_closed = true;
			}
		}

		inline bool closed() const { return _closed; }

		virtual Value toValue() override
		{
			return Value(this, TypeId::UpValue);
		}

	private:

		Value *value;
		bool _closed;

	};

}
