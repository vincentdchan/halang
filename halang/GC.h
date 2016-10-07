#pragma once
#include <memory>
#include "object.h"

namespace halang {

	class GC
	{

	private:

		GCObject* gc;

	protected:

		GC();

	public:

		template<class _Ty, class... _Types> 
		inline _Ty* New(_Types&&... _Args)
		{	
			_Ty* new_obj = new _Ty(std::forward<_Types>(_Args)...);
			new_obj->next = gc;
			gc = new_obj;
			return new_obj;
		}

	};

}
