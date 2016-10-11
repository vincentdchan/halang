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

	SimpleString::SimpleString() : length(0)
	{
		s_value = new char16_t[length + 1];
		s_value[length] = u'\0';

		// calculate hash;
		_hash = 5381;
		int c = 0;

		while ((c = *s_value++))
			_hash = ((_hash << 5) + _hash) + c;

	}

	SimpleString::SimpleString(const std::u16string _str)
	{
		length = _str.size();
		s_value = new char16_t[length + 1];
		
		for (unsigned int i = 0; i < _str.size(); ++i)
			s_value[i] = _str[i];

		s_value[length] = u'\0';

		// calculate hash;
		_hash = 5381;
		int c = 0;

		while ((c = *s_value++))
			_hash = ((_hash << 5) + _hash) + c;
	}

	SimpleString::SimpleString(const SimpleString& _str) :
		_hash(_str._hash), length(_str.length)
	{
		s_value = new char16_t[length + 1];
		s_value[length] = u'\0';

		for (size_type i = 0; i < _str.length; ++i)
			s_value[i] = _str.s_value[i];
	}


	SimpleString::~SimpleString()
	{
		delete s_value;
	}

	char16_t SimpleString::CharAt(unsigned int index) const 
	{
		if (index >= length)
			throw std::runtime_error("<String> index out of range.");
		return s_value[index];
	}

	unsigned int SimpleString::GetHash() const
	{
		return _hash;
	}

	unsigned int SimpleString::GetLength() const
	{
		return length;
	}

	ConsString::ConsString(String* _left, String* _right) :
		left(_left), right(_right), _length(0), _hash(nullptr)
	{
		if (left != nullptr)
			_length += left->GetLength();
		if (right != nullptr)
			_length += right->GetLength();
	}

	ConsString::size_type ConsString::GetLength() const
	{
		return _length;
	}

	unsigned int ConsString::GetHash() const
	{
		if (_hash != nullptr)
			return *_hash;
		else
		{
			auto mThis = const_cast<ConsString*>(this);
			mThis->_hash = new unsigned int(0);

			*mThis->_hash = 5381;
			int c = 0;

			for (size_type i = 0; i < GetLength(); ++i)
			{
				c = CharAt(i);
				*mThis->_hash = ((*mThis->_hash << 5) + *mThis->_hash) + c;
			}
			return *mThis->_hash;

		}
	}

	char16_t ConsString::CharAt(unsigned int index) const
	{
		if (index >= GetLength())
			throw std::runtime_error("<ConsString>index out of range");
		if (left != nullptr)
			if (index < left->GetLength())
				return left->CharAt(index);
			else
				return right->CharAt(index - left->GetLength());
		else
			return right->CharAt(index);
	}

	void ConsString::Mark()
	{
		if (left != nullptr)
			left->Mark();
		if (right != nullptr)
			right->Mark();
	}

	ConsString::~ConsString()
	{
		if (_hash != nullptr)
			delete _hash;
	}

	SliceString::SliceString(String* _src, unsigned int _begin, unsigned int _end):
		source(_src), begin(_begin), end(_end)
	{
		_hash = 5381;
		int c = 0;

		for (size_type i = 0; i < GetLength(); ++i)
		{
			c = CharAt(i);
			_hash = ((_hash << 5) + _hash) + c;
		}

	}

	SliceString::SliceString(const SliceString& _str):
		source(_str.source), begin(_str.begin), end(_str.end)
	{}

	String::size_type SliceString::GetLength() const
	{
		return end - begin;
	}

	char16_t SliceString::CharAt(unsigned int index) const
	{
		if (index >= GetLength())
			throw std::runtime_error("<SliceString>index out of string");

		return source->CharAt(index + begin);
	}

	unsigned int SliceString::GetHash() const
	{
		return _hash;
	}

	void SliceString::Mark()
	{
		source->Mark();
	}

};
