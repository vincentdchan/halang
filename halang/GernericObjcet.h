#pragma once
#include "Dict.h"
#include "object.h"
#include "svm.h"

const char* PROTOTYPE_NAME = "__prototype__";
const char* EXTEND_NAME = "__extend__";

namespace halang
{

	class Class;

	class GenericObject : public Dict
	{
	public:

		GenericObject()
		{ 
			SetValue(Object(PROTOTYPE_NAME), Object());
		}

		GenericObject(const GenericObject& _obj) :
			Dict(_obj)
		{}

		inline GenericObject* GetPrototype()
		{
			auto obj = GetValue(Object(PROTOTYPE_NAME));
			if (obj.isNul())
				return nullptr;
			else
				return reinterpret_cast<GenericObject*>(obj.value.gc);
		}

		inline void SetPrototype(GenericObject* pt)
		{
			auto key = Object(PROTOTYPE_NAME);
			if (pt == nullptr)
				SetValue(key, Object());
			else
				SetValue(key, Object(pt));
		}

	};


	class Class : public Dict
	{
	public:

		static Object Construct(StackVM* svm, Class& cl);

		Class()
		{
			SetValue(Object("__extend__"), Object());
		}

		void New();

		inline Class* GetExtendedClass()
		{
			auto obj = GetValue(Object(EXTEND_NAME));
			return reinterpret_cast<Class*>(obj.value.gc);
		}

		inline void SetExtendedClass(Class* cl)
		{
			auto key = Object(EXTEND_NAME);
			if (cl == nullptr)
				SetValue(key, Object());
			else
				SetValue(key, Object(cl));
		}

	};

}
