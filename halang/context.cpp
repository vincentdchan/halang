#include "context.h"
#include "GC.h"
#include "Dict.h"
#include "string.h"
#include "function.h"
#include "util.h"
#include <sstream>
#include <iostream>

#define TEXT(T) CreatePersistent(T)
#define TOV(T) T->toValue()
#define SBV(T) StringBuffer::T->toValue()
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

	String* Context::StringBuffer::__ADD__ = nullptr;
	String* Context::StringBuffer::__SUB__ = nullptr;
	String* Context::StringBuffer::__MUL__ = nullptr;
	String* Context::StringBuffer::__DIV__ = nullptr;
	String* Context::StringBuffer::__MOD__ = nullptr;
	String* Context::StringBuffer::__LT__ = nullptr;
	String* Context::StringBuffer::__GT__ = nullptr;
	String* Context::StringBuffer::__LTEQ__ = nullptr;
	String* Context::StringBuffer::__GTEQ__ = nullptr;
	String* Context::StringBuffer::__EQ__ = nullptr;
	String* Context::StringBuffer::__STR__ = nullptr;
	String* Context::StringBuffer::__REVERSE__ = nullptr;
	String* Context::StringBuffer::__AND__ = nullptr;
	String* Context::StringBuffer::__OR__ = nullptr;
	String* Context::StringBuffer::__NOT__ = nullptr;

	String* Context::StringBuffer::CONCAT = nullptr;
	String* Context::StringBuffer::LENGTH = nullptr;
	String* Context::StringBuffer::HASH = nullptr;

	String* Context::StringBuffer::PUSH = nullptr;
	String* Context::StringBuffer::POP = nullptr;
	String* Context::StringBuffer::AT = nullptr;
	String* Context::StringBuffer::GET = nullptr;
	String* Context::StringBuffer::SET = nullptr;
	String* Context::StringBuffer::EXIST = nullptr;

	String* Context::StringBuffer::TRUE = nullptr;
	String* Context::StringBuffer::FALSE = nullptr;

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
		InitializeStringBuffer();

		_null_proto = gc->NewPersistent<Dict>();
		_null_proto->SetValue(SBV(__ADD__),		FUN(_null_str_));

		_bool_proto = gc->NewPersistent<Dict>();
		_bool_proto->SetValue(SBV(__AND__),		FUN(_bl_and_));
		_bool_proto->SetValue(SBV(__OR__),		FUN(_bl_or_));
		_bool_proto->SetValue(SBV(__NOT__),		FUN(_bl_not_));
		_bool_proto->SetValue(SBV(__EQ__),		FUN(_bl_eq_));
		_bool_proto->SetValue(SBV(__STR__),		FUN(_bl_str_));

		_si_proto = gc->NewPersistent<Dict>();
		_si_proto->SetValue(SBV(__ADD__),		FUN(_si_add_));
		_si_proto->SetValue(SBV(__SUB__),		FUN(_si_sub_));
		_si_proto->SetValue(SBV(__MUL__),		FUN(_si_mul_));
		_si_proto->SetValue(SBV(__DIV__),		FUN(_si_div_));
		_si_proto->SetValue(SBV(__MOD__),		FUN(_si_mod_));
		_si_proto->SetValue(SBV(__REVERSE__),	FUN(_si_reverse_));
		_si_proto->SetValue(SBV(__EQ__),			FUN(_si_eq_));
		_si_proto->SetValue(SBV(__GT__),			FUN(_si_gt_));
		_si_proto->SetValue(SBV(__LT__),			FUN(_si_lt_));
		_si_proto->SetValue(SBV(__GTEQ__),		FUN(_si_gteq_));
		_si_proto->SetValue(SBV(__LTEQ__),		FUN(_si_lteq_));
		_si_proto->SetValue(SBV(__STR__),		FUN(_si_str_));

		_num_proto = gc->NewPersistent<Dict>();
		_num_proto->SetValue(SBV(__ADD__),		FUN(_num_add_));
		_num_proto->SetValue(SBV(__SUB__),		FUN(_num_sub_));
		_num_proto->SetValue(SBV(__MUL__),		FUN(_num_mul_));
		_num_proto->SetValue(SBV(__DIV__),		FUN(_num_div_));
		_num_proto->SetValue(SBV(__REVERSE__),	FUN(_num_reverse_));
		_num_proto->SetValue(SBV(__EQ__),		FUN(_num_eq_));
		_num_proto->SetValue(SBV(__GT__),		FUN(_num_gt_));
		_num_proto->SetValue(SBV(__LT__),		FUN(_num_lt_));
		_num_proto->SetValue(SBV(__GTEQ__),		FUN(_num_gteq_));
		_num_proto->SetValue(SBV(__LTEQ__),		FUN(_num_lteq_));
		_num_proto->SetValue(SBV(__STR__),		FUN(_num_str_));
		
		_str_proto = gc->NewPersistent<Dict>();
		_str_proto->SetValue(SBV(__STR__),		FUN(_str_str_));
		_str_proto->SetValue(SBV(__ADD__),		FUN(_str_add_));
		_str_proto->SetValue(SBV(CONCAT),		FUN(_str_add_));
		_str_proto->SetValue(SBV(LENGTH),		FUN(_str_length_));
		_str_proto->SetValue(SBV(HASH),			FUN(_str_hash_));

		_array_proto = gc->NewPersistent<Dict>();
		_array_proto->SetValue(SBV(PUSH),		FUN(_array_push_));
		_array_proto->SetValue(SBV(POP),			FUN(_array_pop_));
		_array_proto->SetValue(SBV(AT),			FUN(_array_at_));
		_array_proto->SetValue(SBV(LENGTH),		FUN(_array_length_));

		_dict_proto = gc->NewPersistent<Dict>();
		_dict_proto->SetValue(SBV(GET),			FUN(_dict_get_));
		_dict_proto->SetValue(SBV(SET),			FUN(_dict_get_));
		_dict_proto->SetValue(SBV(EXIST),		FUN(_dict_get_));
	}

	void Context::InitializeStringBuffer()
	{

		StringBuffer::__ADD__ = TEXT("__add__");
		StringBuffer::__SUB__ = TEXT("__sub__");
		StringBuffer::__MUL__ = TEXT("__mul__");
		StringBuffer::__DIV__ = TEXT("__div__");
		StringBuffer::__MOD__ = TEXT("__mod__");
		StringBuffer::__LT__ = TEXT("__lt__");
		StringBuffer::__GT__ = TEXT("__gt__");
		StringBuffer::__LTEQ__ = TEXT("__lteq__");
		StringBuffer::__GTEQ__ = TEXT("__gteq__");
		StringBuffer::__EQ__ = TEXT("__eq__");
		StringBuffer::__STR__ = TEXT("__str__");
		StringBuffer::__REVERSE__ = TEXT("__reverse__");
		StringBuffer::__AND__ = TEXT("__and__");
		StringBuffer::__OR__ = TEXT("__or__");
		StringBuffer::__NOT__ = TEXT("__not__");

		StringBuffer::CONCAT = TEXT("concat");
		StringBuffer::LENGTH = TEXT("length");
		StringBuffer::HASH = TEXT("hash");

		StringBuffer::PUSH = TEXT("push");
		StringBuffer::POP = TEXT("pop");
		StringBuffer::AT = TEXT("at");
		StringBuffer::GET = TEXT("get");
		StringBuffer::SET = TEXT("set");
		StringBuffer::EXIST = TEXT("exist");

		StringBuffer::TRUE = TEXT("true");
		StringBuffer::FALSE = TEXT("false");

	}

	Value Context::_null_str_(Value self, FunctionArgs& args)
	{
		return TEXT("<Null>")->toValue();
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
			return SBV(TRUE);
		else
			return SBV(FALSE);
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