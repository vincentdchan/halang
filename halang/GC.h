#pragma once
#include <memory>
#include "object.h"

namespace halang {

	class GC final
	{

	private:

		GCObject* objects;

	protected:

		GC();
		~GC();

	public:

		template<class _Ty, class... _Types> 
		inline _Ty* New(_Types&&... _Args)
		{	
			_Ty* new_obj = new _Ty(std::forward<_Types>(_Args)...);
			new_obj->next = objects;
			objects = new_obj;
			return new_obj;
		}

	private:

		GCObject* Erase(GCObject* obj);

		void SweepAll();

	};

}
