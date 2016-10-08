#include "stdafx.h"
#include <cstring>
#include "context.h"
#include "GC.h"
#include "string.h"
#include <string>
#include <locale>
#include <codecvt>

namespace halang
{

	String* String::FromU16String(const std::u16string& _str)
	{
		return Context::GetGC()->New<SimpleString>(_str);
	}

	String* String::FromCharArray(const char* _str)
	{
		std::u16string utf16 = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(_str);
		return FromU16String(utf16);
	}

	String* String::FromStdString(const std::string& _str)
	{
		std::u16string utf16 = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(_str.data());
		return FromU16String(utf16);
	}

	String* String::Concat(String* a, String* b)
	{
		return Context::GetGC()->New<ConsString>(a, b);
	}

	String* String::Slice(String* str, unsigned int begin, unsigned int end)
	{
		return Context::GetGC()->New<SliceString>(str, begin, end);
	}

};
