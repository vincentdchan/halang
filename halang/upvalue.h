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
	public:
		UpValue(Object* _re = nullptr) : value(_re), _closed(false)
		{}

		virtual ~UpValue() 
		{
			if (_closed)
				delete value;
		}

		Object getVal() const
		{
			return *value;
		}

		void setVal(const Object& _obj)
		{
			*value = _obj;
		}

		void close()
		{
			if (!_closed) 
			{
				value = new Object(*value);
				_closed = true;
			}
		}

		inline bool closed() const { return _closed; }

	private:

		Object *value;
		bool _closed;

	};

}
