#pragma once
#include "object.h"

namespace runtime
{
	class UpValue : GCObject
	{
	public:
		UpValue() : refer(nullptr), closed(false)
		{}

		union
		{
			Object value;
			Object *refer;
		};
		bool closed;

	};
}
