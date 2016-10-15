#include "GC.h"
#include "svm.h"
#include "context.h"
#include "Dict.h"
#include <iostream>

namespace halang
{

	GC::GC() :
		objects(nullptr), counter(0)
	{
		Context::gc = this;
	}

	GCObject* GC::Erase(GCObject* obj)
	{
		GCObject* _next = obj->next;
		delete obj;
		return _next;
	}

	void GC::ClearAllMarks()
	{
		// std::cout << "full gc" << std::endl;
		auto ptr = objects;
		while (ptr != nullptr)
		{
			ptr->marked = false;
			ptr = ptr->next;
		}
	}

	void GC::SweepAll()
	{
		GCObject** ptr = &objects;

		while (*ptr != nullptr)
		{
			if (!(*ptr)->marked && !(*ptr)->persistent)
				*ptr = Erase(*ptr);
			else
				ptr = &((*ptr)->next);
		}
	}

	void GC::FullGC()
	{
#ifdef _DEBUG
		// std::cout << "Full GC" << std::endl;
#endif
		ClearAllMarks();
		if (Context::GetVM()->sc != nullptr)
			Context::GetVM()->sc->Mark();
		SweepAll();
	}

	void GC::CheckAndGC()
	{
		if (counter >= 64)
		{
			FullGC();
			counter = 0;
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
