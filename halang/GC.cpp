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
		GCObject* _next = obj->next;
		delete obj;
		return _next;
	}

	void GC::SweepAll()
	{
		GCObject** ptr = &objects;

		while (*ptr != nullptr)
		{
			if (!(*ptr)->marked)
				*ptr = Erase(*ptr);
			else
				ptr = &((*ptr)->next);
		}
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
