#include "context.h"
#include "GC.h"
#include "Dict.h"
#include "string.h"
#include "function.h"
#include "util.h"
#include <sstream>
#include <iostream>

#define TEXT(T) CreatePersistent(T)->toValue()
#define FUN(F) gc->NewPersistent<Function>(F)->toValue()

namespace halang
{

	GC* Context::GetGC() { return gc; }
	StackVM* Context::GetVM() { return vm; }

	Dict* Context::GetNullPrototype() { return _null_proto; }

	Dict* Context::GetBoolPrototype() { return _bool_proto; } 

	Dict* Context::GetSmallIntPrototype() { return _si_proto; }

	Dict* Context::GetNumberPrototype() { return _num_proto; }

	Dict* Context::GetStringPrototype() { return _str_proto; }
		
	GC* Context::gc = nullptr;
	StackVM* Context::vm = nullptr;

	Dict* Context::_null_proto = nullptr;
	Dict* Context::_bool_proto = nullptr;
	Dict* Context::_si_proto = nullptr;
	Dict* Context::_num_proto = nullptr;
	Dict* Context::_str_proto = nullptr;
	Dict* Context::_array_proto = nullptr;
	Dict* Context::_dict_proto = nullptr;

	String* Context::CreatePersistent(const char* _s)
	{
		auto s = String::FromCharArray(_s);
		s->persistent = true;
		return s;
	}

	void Context::InitializeDefaultPrototype()
	{
		_null_proto = gc->NewPersistent<Dict>();
		_null_proto->SetValue(TEXT("__str__"),		FUN(_null_str_));

		_bool_proto = gc->NewPersistent<Dict>();
		_bool_proto->SetValue(TEXT("__and__"),		FUN(_bl_and_));
		_bool_proto->SetValue(TEXT("__or__"),		FUN(_bl_or_));
		_bool_proto->SetValue(TEXT("__not__"),		FUN(_bl_not_));
		_bool_proto->SetValue(TEXT("__eq__"),		FUN(_bl_eq_));
		_bool_proto->SetValue(TEXT("__str__"),		FUN(_bl_str_));

		_si_proto = gc->NewPersistent<Dict>();
		_si_proto->SetValue(TEXT("__add__"),		FUN(_si_add_));
		_si_proto->SetValue(TEXT("__sub__"),		FUN(_si_sub_));
		_si_proto->SetValue(TEXT("__mul__"),		FUN(_si_mul_));
		_si_proto->SetValue(TEXT("__div__"),		FUN(_si_div_));
		_si_proto->SetValue(TEXT("__mod__"),		FUN(_si_mod_));
		_si_proto->SetValue(TEXT("__reverse__"),	FUN(_si_reverse_));
		_si_proto->SetValue(TEXT("__eq__"),			FUN(_si_eq_));
		_si_proto->SetValue(TEXT("__gt__"),			FUN(_si_gt_));
		_si_proto->SetValue(TEXT("__lt__"),			FUN(_si_lt_));
		_si_proto->SetValue(TEXT("__gteq__"),		FUN(_si_gteq_));
		_si_proto->SetValue(TEXT("__lteq__"),		FUN(_si_lteq_));
		_si_proto->SetValue(TEXT("__str__"),		FUN(_si_str_));

		_num_proto = gc->NewPersistent<Dict>();
		_num_proto->SetValue(TEXT("__add__"),		FUN(_num_add_));
		_num_proto->SetValue(TEXT("__sub__"),		FUN(_num_sub_));
		_num_proto->SetValue(TEXT("__mul__"),		FUN(_num_mul_));
		_num_proto->SetValue(TEXT("__div__"),		FUN(_num_div_));
		_num_proto->SetValue(TEXT("__reverse__"),	FUN(_num_reverse_));
		_num_proto->SetValue(TEXT("__eq__"),		FUN(_num_eq_));
		_num_proto->SetValue(TEXT("__gt__"),		FUN(_num_gt_));
		_num_proto->SetValue(TEXT("__lt__"),		FUN(_num_lt_));
		_num_proto->SetValue(TEXT("__gteq__"),		FUN(_num_gteq_));
		_num_proto->SetValue(TEXT("__lteq__"),		FUN(_num_lteq_));
		_num_proto->SetValue(TEXT("__str__"),		FUN(_num_str_));
		
		_str_proto = gc->NewPersistent<Dict>();
		_str_proto->SetValue(TEXT("__str__"),		FUN(_str_str_));
		_str_proto->SetValue(TEXT("__add__"),		FUN(_str_add_));
		_str_proto->SetValue(TEXT("concat"),		FUN(_str_add_));
		_str_proto->SetValue(TEXT("length"),		FUN(_str_length_));
		_str_proto->SetValue(TEXT("hash"),			FUN(_str_hash_));

		_array_proto = gc->NewPersistent<Dict>();
		_array_proto->SetValue(TEXT("push"),		FUN(_array_push_));
		_array_proto->SetValue(TEXT("pop"),			FUN(_array_pop_));
		_array_proto->SetValue(TEXT("at"),			FUN(_array_at_));
		_array_proto->SetValue(TEXT("length"),		FUN(_array_length_));

		_dict_proto = gc->NewPersistent<Dict>();
		_dict_proto->SetValue(TEXT("get"),			FUN(_dict_get_));
		_dict_proto->SetValue(TEXT("set"),			FUN(_dict_get_));
		_dict_proto->SetValue(TEXT("exisit"),		FUN(_dict_get_));
	}

	Value Context::_null_str_(Value self, FunctionArgs& args)
	{
		return TEXT("<Null>");
	}

	Value Context::_bl_and_(Value self, FunctionArgs& args)
	{
		return Value(self.value.bl && args[0].value.bl);
	}

	Value Context::_bl_or_(Value self, FunctionArgs& args)
	{
		return Value(self.value.bl || args[0].value.bl);
	}

	Value Context::_bl_not_(Value self, FunctionArgs& args)
	{
		return Value(!self.value.bl);
	}

	Value Context::_bl_eq_(Value self, FunctionArgs& args)
	{
		return Value(self.value.bl == args[0].value.bl);
	}

	Value Context::_bl_str_(Value self, FunctionArgs& args)
	{
		if (self.value.bl)
			return TEXT("True");
		else
			return TEXT("False");
	}

	Value Context::_si_add_(Value self, FunctionArgs& args)
	{
		// ASSERT(self.type == TypeID::SmallInt)
		auto arg = args.At(0);
		return Value(self . value.si + arg.value.si);
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

	Value Context::_si_reverse_(Value self, FunctionArgs& args)
	{
		return Value(self.value.si * -1);
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

	Value Context::_num_reverse_(Value self, FunctionArgs& args)
	{
		return Value(self.value.number * -1.0);
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

	Value Context::_str_str_(Value self, FunctionArgs& args)
	{
		return self;
	}

	Value Context::_print_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		if (arg.type != TypeId::String)
			throw std::runtime_error("You must print a string");
		auto _str = reinterpret_cast<String*>(arg.value.gc);
		std::u16string utf16;
		_str->ToU16String(utf16);
		std::cout << utils::UTF16_to_UTF8(utf16) << std::endl;
		return Value();
	}

	Value Context::_str_add_(Value self, FunctionArgs& args)
	{
		auto arg = args.At(0);
		if (arg.type != TypeId::String)
			throw std::runtime_error("You can only and string to another string");
		auto _self_str = reinterpret_cast<String*>(self.value.gc);
		auto _that_str = reinterpret_cast<String*>(arg.value.gc);
		return String::Concat(_self_str, _that_str)->toValue();
	}

	Value Context::_str_length_(Value self, FunctionArgs& args)
	{
		if (self.type != TypeId::String)
			throw std::runtime_error("Not a string");
		return Value(static_cast<TSmallInt>(
			reinterpret_cast<String*>(self.value.gc)->GetLength()));
	}

	Value Context::_str_hash_(Value self, FunctionArgs& args)
	{
		if (self.type != TypeId::String)
			throw std::runtime_error("Not a string");
		return Value(static_cast<TSmallInt>(
			reinterpret_cast<String*>(self.value.gc)->GetHash()));
	}

	Value Context::_array_push_(Value self, FunctionArgs& args)
	{
		if (args.GetLength() < 1)
			std::runtime_error("arguments not enough.");
		auto arr = reinterpret_cast<Array*>(self.value.gc);
		arr->Push(args[0]);
		return Value();
	}

	Value Context::_array_pop_(Value self, FunctionArgs& args)
	{
		auto arr = reinterpret_cast<Array*>(self.value.gc);
		return arr->Pop();
	}

	Value Context::_array_at_(Value self, FunctionArgs& args)
	{
		if (args.GetLength() < 1)
			std::runtime_error("arguments not enough.");
		auto arg1 = args[0];
		if (arg1.type != TypeId::SmallInt)
			std::runtime_error("index must be int");
		auto arr = reinterpret_cast<Array*>(self.value.gc);
		return arr->At(arg1.value.si);
	}

	Value Context::_array_length_(Value self, FunctionArgs& args)
	{
		auto arr = reinterpret_cast<Array*>(self.value.gc);
		return Value(static_cast<TSmallInt>(arr->GetLength()));
	}

	Value Context::_dict_set_(Value self, FunctionArgs& args)
	{
		if (args.GetLength() < 2)
			throw std::runtime_error("arguments not enough");
		auto _dict = reinterpret_cast<Dict*>(self.value.gc);
		_dict->SetValue(args[0], args[1]);
		return Value();
	}

	Value Context::_dict_get_(Value self, FunctionArgs& args)
	{
		if (args.GetLength() < 1)
			throw std::runtime_error("arguments not enough");
		auto _dict = reinterpret_cast<Dict*>(self.value.gc);
		return Value(_dict->GetValue(args[0]));
	}

	Value Context::_dict_exist_(Value self, FunctionArgs& args)
	{
		if (args.GetLength() < 1)
			throw std::runtime_error("arguments not enough");
		auto _dict = reinterpret_cast<Dict*>(self.value.gc);
		return Value(_dict->Exist(args[0]));
	}

}