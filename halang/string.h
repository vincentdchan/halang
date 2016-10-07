#pragma once
#include <ostream>
#include <functional>
#include <vector>
#include <cinttypes>
#include <string>
#include "object.h"
#include "halang.h"

namespace halang
{

	class String : public GCObject
	{
	public:

		static String* FromU16String(const std::u16string&);
		static String* FromStdString(const std::string&);
		static String* FromCharArray(const char*);
		static String* Slice(String*, unsigned int begin, unsigned int end);
		static String* Concat(String*, String*);

		virtual Value GetValue() override { return Value(this, TypeId::String); }
		virtual char16_t CharAt(unsigned int) const = 0;
		virtual unsigned int GetHash() const = 0;

	};

	class SimpleString : 
		public String, private std::u16string
	{
	private:

		unsigned int _hash;

	protected:

		SimpleString()
		{
			_hash = std::hash<std::u16string>()(*this);
		}

		SimpleString(const std::u16string _str) :
			u16string(_str)
		{
			_hash = std::hash<std::u16string>()(*this);
		}

		SimpleString(const SimpleString& _str) :
			u16string(_str), _hash(_str._hash)
		{}

	public:

		virtual char16_t CharAt(unsigned int index) const override 
		{
			return std::u16string::at(index);
		}

		virtual unsigned int GetHash() const override
		{
			return _hash;
		}

	};


};

namespace std
{
	template<>
	struct hash<halang::String>
	{
		unsigned int operator()(const halang::String& _Str)
		{
			return _Str.GetHash();
		}
	};
};
