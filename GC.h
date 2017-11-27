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

		friend class StackVM;

		template<class _Ty, class... _Types> 
		inline _Ty* New(_Types&&... _Args)
		{	
			_Ty* new_obj = new _Ty(std::forward<_Types>(_Args)...);
			new_obj->next = objects;
			objects = new_obj;

			counter++;

			return new_obj;
		}

		template<class _Ty, class... _Types> 
		inline _Ty* NewPersistent(_Types&&... _Args)
		{	
			_Ty* new_obj = new _Ty(std::forward<_Types>(_Args)...);
			new_obj->next = objects;
			new_obj->persistent = true;
			objects = new_obj;

			return new_obj;
		}

		void CheckAndGC();

	private:

		GCObject* Erase(GCObject* obj);

		unsigned int counter;
		void FullGC();
		void ClearAllMarks();
		void SweepAll();

	};

}
