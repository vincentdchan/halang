#include "stdafx.h"
#include <cstring>
#include <codecvt>
#include <locale>
#include "string.h"
#include "context.h"
#include "GC.h"

namespace halang
{

	// return a Simple String
	SimpleString* String::FromU16String(const std::u16string& str)
	{
		auto gc = Context::GetGc();

		auto new_str = gc->New<SimpleString>(str.c_str());
		return new_str;
	}

	// From UTF-8 Multi-Bytes format to UTF-16 
	SimpleString* String::FromCStr(const char* _cstr)
	{
		std::u16string utf16 = 
			std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(_cstr);
		return FromU16String(utf16);
	}

	SimpleString* String::FromStdString(const std::string& str)
	{
		std::u16string utf16 = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(str.data());
		return FromU16String(utf16);
	}

};
