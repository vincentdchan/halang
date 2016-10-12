#include "stdafx.h"
#include "context.h"
#include "object.h"
#include "string.h"
#include "svm.h"


namespace halang
{
	Value GCObject::toValue() 
	{ 
		return Value(); 
	}

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

	bool Value::operator==(const Value& that) const
	{
		switch (type)
		{
		case halang::TypeId::Null:
			return true;
		case halang::TypeId::Bool:
			return value.bl == that.value.bl;
		case halang::TypeId::SmallInt:
			return value.si == that.value.si;
		case halang::TypeId::Number:
			return value.si == that.value.number;
		case halang::TypeId::String:
		{
			auto s1 = reinterpret_cast<String*>(value.gc);
			auto s2 = reinterpret_cast<String*>(that.value.gc);

			return s1->GetHash() == s1->GetHash();
		}
		default:
			throw std::runtime_error("wrong type");
		}
	}

};
