#include "stdafx.h"
#include "object.h"

namespace runtime
{

	TNumber toNumber(Object _obj)
	{
		if (_obj.isNumber())
			return _obj.value.number;
		else if (_obj.isSmallInt())
			return static_cast<TNumber>(_obj.value.si);
		else
			return 0.0;
	}

	std::ostream& operator<<(std::ostream& _ost, Object _obj)
	{
		switch (_obj.type)
		{
		case Object::TYPE::BOOL:
			return _ost << (_obj.value.bl ? STRUE : SFALSE);
		case Object::TYPE::GC:
			return _ost << "<Object>";
		case Object::TYPE::NUL:
			return _ost << "null";
		case Object::TYPE::NUMBER:
			return _ost << _obj.value.number;
		case Object::TYPE::SMALL_INT:
			return _ost << _obj.value.si;
		case Object::TYPE::STRING:
			return _ost << "<STRING>";
		}
		return _ost;
	}
};