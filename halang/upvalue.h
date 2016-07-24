#pragma once
#include "object.h"

namespace runtime
{
	class UpValue : GCObject
	{
	public:
		Object value;
	};
}
