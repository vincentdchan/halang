#pragma once
#include "object.h"

namespace halang
{

	class UpValue : public GCObject
	{
	public:
		UpValue(Object* _re = nullptr) : refer(_re), _closed(false)
		{}

		Object getVal() const
		{
			if (_closed)
				return value;
			else
				return *refer;
		}

		void setVal(const Object& _obj)
		{
			if (_closed)
				value = _obj;
			else
				*refer = _obj;
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
		virtual ~UpValue() {}
	private:

		union
		{
			Object value;
			Object *refer;
		};

		bool _closed;
	};

}
