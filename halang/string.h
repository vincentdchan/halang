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

		typedef unsigned int size_type;

		static String* FromU16String(const std::u16string&);
		static String* FromStdString(const std::string&);
		static String* FromCharArray(const char*);
		static String* Concat(String*, String*);
		static String* Slice(String*, unsigned int begin, unsigned int end);

		virtual void Mark() override {}
		virtual Value toValue() override { return Value(this, TypeId::String); }
		virtual char16_t CharAt(unsigned int) const = 0;
		virtual unsigned int GetHash() const = 0;
		virtual size_type GetLength() const = 0;
		virtual Dict* GetPrototype() override { return nullptr; }
		virtual ~String() {}

	};

	class SimpleString : public String
	{
	public:

		friend class GC;

	private:

		unsigned int _hash;
		char16_t* s_value;
		size_type length;

	protected:

		SimpleString();
		SimpleString(const std::u16string _str);
		SimpleString(const SimpleString& _str);

	public:

		virtual ~SimpleString();
		virtual char16_t CharAt(unsigned int index) const override;
		virtual unsigned int GetHash() const override;
		virtual unsigned int GetLength() const override;

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

		ConsString(String* _left = nullptr, String* _right = nullptr);

	public:

		virtual size_type GetLength() const override;

		/// <summay>
		/// calculate hash
		/// calculate when necessary
		/// </summary>
		virtual unsigned int GetHash() const override;
		virtual char16_t CharAt(unsigned int index) const override;
		virtual void Mark() override;
		virtual ~ConsString();
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

		SliceString(String* _src, unsigned int _begin, unsigned int _end);
		SliceString(const SliceString& _str);

	public:

		virtual size_type GetLength() const override;
		virtual char16_t CharAt(unsigned int index) const override;
		virtual unsigned int GetHash() const override;
		virtual void Mark() override;

	};

};

namespace std
{
	using namespace halang;

	template<>
	struct hash<halang::String>
	{
		unsigned int operator()(const halang::String& _Str) const
		{
			return _Str.GetHash();
		}
	};

	template<>
	struct hash<Value>
	{

		unsigned int operator()(const Value& v) const
		{
			switch (v.type)
			{
			case TypeId::Null:
				return 0;
			case TypeId::SmallInt:
				return hash<TSmallInt>{}(v.value.si);
			case TypeId::Number:
				return hash<TNumber>{}(v.value.number);
			case TypeId::String:
				return hash<halang::String>{}(*reinterpret_cast<String*>(v.value.si));
			default:
				throw std::runtime_error("do hash to wrong type");
				return 0;
			}
		}

	};

};
