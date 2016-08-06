#include "stdafx.h"
#include <cstring>
#include "string.h"

namespace halang
{

	unsigned int IString::calculateHash(const char* s)
	{
		unsigned int _hash = 5381;
		int c = 0;

		while ((c = *s++))
			_hash = ((_hash << 5) + _hash) + c;
		return _hash;
	}

	IString::IString() : _hash(0), _length(0)
	{
		_ref_data = new ReferData(1);
		_str = new char[1];
		*_str = '\0';
	}

	IString::IString(const char* _Str)
	{
		auto _len = strlen(_Str);
		_str = new char[_len + 1];
		memset(_str, 0, _len + 1);
		memcpy(_str, _Str, _len);

		_hash = calculateHash(_str);
		_length = _len;

		_ref_data = new ReferData(1);
	}

	IString::IString(const std::string& _Str) :
		IString(_Str.c_str())
	{ }

	// move constructor
	IString::IString(IString&& _Str)
	{
		_hash = _Str._hash;
		_length = _Str._length;
		_ref_data = _Str._ref_data;
		_str = _Str._str;

		_Str._ref_data = nullptr;
		_Str._str = nullptr;
	}

	IString::IString(const IString& _IStr) :
		_hash(_IStr._hash), _length(_IStr._length),
		_str(_IStr._str), _ref_data(_IStr._ref_data)
	{
		_ref_data->count++;
	}

	IString IString::operator+(IString _is) const
	{
		return this->operator+(_is.c_str());
	}

	IString IString::operator+(const char* _Str) const
	{
		unsigned int s2_len = strlen(_Str);
		char* tmp = new char[_length + s2_len + 1];
		memset(tmp, 0, _length + s2_len + 1);

		memcpy(tmp, this->_str, _length);
		memcpy(tmp + _length, _Str, s2_len);
		IString new_str(tmp);
		delete[] tmp;
		return new_str;
	}

	IString& IString::operator=(IString _is)
	{
		if (_str == _is._str) return *this;

		_ref_data->count--;
		if (_ref_data->count == 0)
		{
			delete _ref_data;
			delete[] _str;
		}
		_ref_data = _is._ref_data;
		_str = _is._str;
		_ref_data->count++;

		_hash = _is._hash;
		_length = _is._length;

		return *this;
	}

	IString::~IString()
	{
		if (_ref_data)
		{
			_ref_data->count--;
			if (_ref_data->count == 0)
			{
				delete[] _str;
				delete _ref_data;
			}
		}
	}

	std::string IString::getStdString() const
	{
		return std::string(_str);
	}

};
