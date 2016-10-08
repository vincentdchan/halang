#include "stdafx.h"
#include "context.h"
#include "object.h"
#include "string.h"
#include "svm.h"


namespace halang
{

	Dict* Value::GetPrototype()
	{
		switch (type)
		{
		case halang::TypeId::Null:
			return Context::GetNullPrototype();
		case halang::TypeId::Bool:
			return Context::GetBoolPrototype();
		case halang::TypeId::SmallInt:
			return Context::GetSmallIntPrototype();
		case halang::TypeId::Number:
			return Context::GetNumberPrototype();
		case halang::TypeId::GCObject:
		case halang::TypeId::String:
			return value.gc->GetPrototype();
		default:
			throw std::runtime_error("<Value>Prototype not found.");
		}
	}

};
