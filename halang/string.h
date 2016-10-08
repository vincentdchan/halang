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

	class String;
	class ConsString;
	class SliceString;

	class String : public GCObject
	{
	public:

		typedef unsigned int size_type;

		static String* FromU16String(const std::u16string&);
		static String* FromStdString(const std::string&);
		static String* FromCharArray(const char*);
		static String* Concat(String*, String*);
		static String* Slice(String*, unsigned int begin, unsigned int end);

		virtual void Mark() override {}
		virtual Value GetValue() override { return Value(this, TypeId::String); }
		virtual char16_t CharAt(unsigned int) const = 0;
		virtual unsigned int GetHash() const = 0;
		virtual size_type GetLength() const = 0;
		virtual ~String() {}

	};

	class SimpleString : public String
	{

	private:

		unsigned int _hash;
		char16_t* s_value;
		size_type length;

	protected:

		SimpleString() : length(0)
		{
			s_value = new char16_t[length + 1];
			s_value[length] = u'\0';

			// calculate hash;
			_hash = 5381;
			int c = 0;

			while ((c = *s_value++))
				_hash = ((_hash << 5) + _hash) + c;

		}

		SimpleString(const std::u16string _str)
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

		SimpleString(const SimpleString& _str) :
			_hash(_str._hash), length(_str.length)
		{
			s_value = new char16_t[length + 1];
			s_value[length] = u'\0';

			for (size_type i = 0; i < _str.length; ++i)
				s_value[i] = _str.s_value[i];
		}

	public:

		virtual ~SimpleString()
		{
			delete s_value;
		}

		virtual char16_t CharAt(unsigned int index) const override 
		{
			if (index >= length)
				throw std::runtime_error("<String> index out of range.");
			return s_value[index];
		}

		virtual unsigned int GetHash() const override
		{
			return _hash;
		}

		virtual unsigned int GetLength() const override
		{
			return length;
		}

	};

	class ConsString : public String
	{
	public:

		friend class GC;
		friend class StackVM;

	private:

		String* left;
		String* right;
		unsigned int* _hash;
		size_type _length;

	protected:

		ConsString(String* _left = nullptr, String* _right = nullptr) :
			left(_left), right(_right), _length(0), _hash(nullptr)
		{
			if (left != nullptr)
				_length += left->GetLength();
			if (right != nullptr)
				_length += right->GetLength();
		}

	public:

		virtual size_type GetLength() const override
		{
			return _length;
		}

		/// <summay>
		/// calculate hash
		/// calculate when necessary
		/// </summary>
		virtual unsigned int GetHash() const override
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

		virtual char16_t CharAt(unsigned int index) const override
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

		virtual void Mark() override
		{
			if (left != nullptr)
				left->Mark();
			if (right != nullptr)
				right->Mark();
		}

		virtual ~ConsString()
		{
			if (_hash != nullptr)
				delete _hash;
		}

	};

	class SliceString : public String
	{
	public:

		friend class GC;
		friend class StackVM;

	private:

		String* source;
		unsigned int begin, end;
		unsigned int _hash;

	protected:

		SliceString(String* _src, unsigned int _begin, unsigned int _end):
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

		SliceString(const SliceString& _str):
			source(_str.source), begin(_str.begin), end(_str.end)
		{}

	public:

		virtual size_type GetLength() const override
		{
			return end - begin;
		}

		virtual char16_t CharAt(unsigned int index) const override
		{
			if (index >= GetLength())
				throw std::runtime_error("<SliceString>index out of string");

			return source->CharAt(index + begin);
		}

		virtual unsigned int GetHash() const override
		{
			return _hash;
		}

		virtual void Mark() override
		{
			source->Mark();
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
