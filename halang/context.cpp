#include "context.h"
#include "GC.h"
#include "Dict.h"
#include "string.h"
#include "function.h"
#include <sstream>

#define TEXT(T) String::FromCharArray(T)->toValue()
#define FUN(F, A) gc->New<Function>(F, A)->toValue()

namespace halang
{

	GC* Context::GetGC() { return gc; }

	Dict* Context::GetNullPrototype()
	{
		return _null_proto;
	}

	Dict* Context::GetBoolPrototype()
	{
		return _bool_proto;
	}

	Dict* Context::GetSmallIntPrototype()
	{
		return _si_proto;
	}

	Dict* Context::GetNumberPrototype()
	{
		return _num_proto;
	}
		
	GC* Context::gc = nullptr;
	Dict* Context::_null_proto = nullptr;
	Dict* Context::_bool_proto = nullptr;
	Dict* Context::_si_proto = nullptr;
	Dict* Context::_num_proto = nullptr;

	void Context::InitializeDefaultPrototype()
	{
		_null_proto = gc->New<Dict>();
		_null_proto->SetValue(TEXT("__str__"), FUN(_null_str_, 0));

		_si_proto = gc->New<Dict>();
		_si_proto->SetValue(TEXT("__add__"),	FUN(_si_add_, 1));
		_si_proto->SetValue(TEXT("__sub__"),	FUN(_si_sub_, 1));
		_si_proto->SetValue(TEXT("__mul__"),	FUN(_si_mul_, 1));
		_si_proto->SetValue(TEXT("__div__"),	FUN(_si_div_, 1));
		_si_proto->SetValue(TEXT("__mod__"),	FUN(_si_mod_, 1));
		_si_proto->SetValue(TEXT("__eq__"),		FUN(_si_eq_, 0));
		_si_proto->SetValue(TEXT("__gt__"),		FUN(_si_gt_, 0));
		_si_proto->SetValue(TEXT("__lt__"),		FUN(_si_lt_, 0));
		_si_proto->SetValue(TEXT("__gteq__"),	FUN(_si_gteq_, 0));
		_si_proto->SetValue(TEXT("__lteq__"),	FUN(_si_lteq_, 0));
		_si_proto->SetValue(TEXT("__str__"),	FUN(_si_str_, 0));

		_num_proto = gc->New<Dict>();
		_num_proto->SetValue(TEXT("__add__"),	FUN(_num_add_, 1));
		_num_proto->SetValue(TEXT("__sub__"),	FUN(_num_sub_, 1));
		_num_proto->SetValue(TEXT("__mul__"),	FUN(_num_mul_, 1));
		_num_proto->SetValue(TEXT("__div__"),	FUN(_num_div_, 1));
		_num_proto->SetValue(TEXT("__eq__"),	FUN(_num_eq_, 0));
		_num_proto->SetValue(TEXT("__gt__"),	FUN(_num_gt_, 0));
		_num_proto->SetValue(TEXT("__lt__"),	FUN(_num_lt_, 0));
		_num_proto->SetValue(TEXT("__gteq__"),	FUN(_num_gteq_, 0));
		_num_proto->SetValue(TEXT("__lteq__"),	FUN(_num_lteq_, 0));
		_num_proto->SetValue(TEXT("__str__"),	FUN(_num_str_, 0));

	}

	Value Context::_null_str_(Value self, FunctionArgs& args)
	{
		return TEXT("<NULL>");
	}

	Value Context::_si_add_(Value self, FunctionArgs& args)
	{
		// ASSERT(self.type == TypeID::SmallInt)
		auto arg = args.At(0);
		return Value(self.value.si + arg.value.si);
	}

	Value Context::_si_sub_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		return Value(self.value.si - arg.value.si);
	}

	Value Context::_si_mul_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		return Value(self.value.si * arg.value.si);
	}

	Value Context::_si_div_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		return Value(self.value.si / arg.value.si);
	}

	Value Context::_si_mod_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		return Value(self.value.si % arg.value.si);
	}

	Value Context::_si_eq_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		return Value(self.value.si == arg.value.si);
	}

	Value Context::_si_gt_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		return Value(self.value.si > arg.value.si);
	}

	Value Context::_si_lt_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		return Value(self.value.si < arg.value.si);
	}

	Value Context::_si_gteq_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		return Value(self.value.si >= arg.value.si);
	}

	Value Context::_si_lteq_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		return Value(self.value.si <= arg.value.si);
	}

	Value Context::_si_str_(Value self, FunctionArgs& args)
	{
		std::stringstream ss;
		ss << "<int: " << self.value.si << ">";
		return String::FromStdString(ss.str())->toValue();
	}

	Value Context::_num_add_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		return Value(self.value.number + arg.value.number);
	}

	Value Context::_num_sub_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		return Value(self.value.number - arg.value.number);
	}

	Value Context::_num_mul_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		return Value(self.value.number * arg.value.number);
	}

	Value Context::_num_div_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		return Value(self.value.number / arg.value.number);
	}

	Value Context::_num_eq_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		return Value(self.value.si == arg.value.si);
	}

	Value Context::_num_gt_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		return Value(self.value.number > arg.value.number);
	}

	Value Context::_num_lt_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		return Value(self.value.number < arg.value.number);
	}

	Value Context::_num_gteq_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		return Value(self.value.number >= arg.value.number);
	}

	Value Context::_num_lteq_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		return Value(self.value.number <= arg.value.number);
	}

	Value Context::_num_str_(Value self, FunctionArgs& args)
	{
		std::stringstream ss;
		ss << "<number: " << self.value.number << ">";
		return String::FromStdString(ss.str())->toValue();
	}

}