#include "GC.h"
#include "context.h"

namespace halang
{

	GC::GC() :
		gc(nullptr)
	{
		Context::gc = this;
	}

}
