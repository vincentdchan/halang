#include "GC.h"
#include "context.h"

namespace halang
{

	GC::GC() :
		objects(nullptr)
	{
		Context::gc = this;
	}

	GCObject* GC::Erase(GCObject* obj)
	{
		auto _next = obj->next;
		delete obj;
		return _next;
	}

	GC::~GC()
	{
		// clear all objects
		while (objects != nullptr)
		{
			objects = Erase(objects);
		}
	}

}
