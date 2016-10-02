#pragma once
#include <ostream>
#include <functional>
#include <vector>
#include <cinttypes>
#include "halang.h"
#include "object.h"
#include "context.h"

namespace halang
{

	class String : Object
	{
	protected:
		// 00 for simple string
		// 01 for rope string
		// 10 for slice string
		unsigned int stringType : 2;

	public:
		
		typedef std::uint32_t size_type;
		typedef std::uint32_t hash_type;

		static String* Concat(const String&, const String&);
		static String* Slice(const String&, unsigned int begin, unsigned int end);

		virtual Object* GetPrototype() const override { return nullptr; }

		virtual size_type GetLength() const = 0;
		virtual hash_type GetHash() const = 0;
		virtual TChar CharAt(unsigned int) const = 0;

		virtual void Mark() override {}

		inline bool isSimpleString() const { return stringType == 0; }
		inline bool isRopeString() const { return stringType == 1; }
		inline bool isSliceString() const { return stringType == 2; }
		
	};

	static bool operator==(const String& str1, const String& str2)
	{
		return str1.GetHash() == str2.GetHash();
	}

	class SimpleString : String
	{
	private:

		static size_type TCharLen(const TChar* str)
		{
			size_type size = 0;
			while (str++ != 0) size++;
			return size;
		}
		
	public:

		virtual unsigned int GetLength() const override
		{
			return length;
		}

		virtual unsigned int GetHash() const override
		{
			return hash;
		}

		virtual void Mark() override
		{
			GC::SetMark(v_array);
		}

	protected:

		SimpleString(const TChar* str)
		{
			auto alloc_size = TCharLen(str);
			v_array = Context::GetGc()->NewTChar(alloc_size + 1);
			for (unsigned int i = 0; i < alloc_size; ++i)
				v_array[i] = str[i];
			length = alloc_size;

			hash = 5381;
			TChar c = 0;

			while ((c = *v_array++))
				hash = ((hash << 5) + hash) + c;

		}

	private:

		TChar* v_array;
		hash_type hash;
		size_type length;
		size_type capacity;
	};

	class ConsString : String
	{
	protected:

		ConsString() :
			p_hash(nullptr), p_length(nullptr)
		{}

	public:

		virtual size_type GetLength() const override
		{
			if (p_length != nullptr)
				return *p_length;
			else
			{
				auto m_this = const_cast<ConsString*>(this);
				m_this->p_length = Context::GetGc()->NewUInt32(0);

				if (left != nullptr)
					*(m_this->p_length) += left->GetLength();
				if (right != nullptr)
					*(m_this->p_length) += right->GetLength();

				return *(m_this->p_length);
			}
		}

		virtual TChar CharAt(unsigned int index) const override
		{
			if (index < GetLength())
				throw std::runtime_error("Index out of range.");

			if (left == nullptr)
				return right->CharAt(index);
			else
			{
				if (index >= left->GetLength())
					return right->CharAt(index - left->GetHash());
				else
					return left->CharAt(index);
			}
		}

		virtual hash_type GetHash() const override
		{
			if (p_hash != nullptr)
				return *p_hash;
			{
				auto m_this = const_cast<ConsString*>(this);
				m_this->p_hash = Context::GetGc()->NewUInt32(5381);

				for (unsigned int i = 0; i < GetLength(); ++i)
				{
					*(m_this->p_hash) = (*(m_this->p_hash) << 5) + *(m_this->p_hash) + CharAt(i);
				}

				return *m_this->p_hash;
			}

		}

		virtual void Mark() override
		{
			if (left != nullptr)
				GC::SetMark(left);
			if (right != nullptr)
				GC::SetMark(right);
			if (p_hash != nullptr)
				GC::SetMark(p_hash);
			if (p_length != nullptr)
				GC::SetMark(p_length);
		}


	private:

		String* left;
		String* right;

		hash_type * p_hash;
		size_type * p_length;

	};

	class SliceString : String
	{

	public:

		virtual unsigned int GetLength() const override
		{
			return end - begin;
		}

		virtual unsigned int GetHash() const override
		{
			return hash;
		}

		virtual TChar CharAt(unsigned int index) const override
		{
			return source->CharAt(this->begin + index);
		}

	protected:

		SliceString(String * _src, 
			unsigned int _begin, unsigned int _end) :
			source(_src), begin(_begin), end(_end)
		{

			hash = 5381;
			int c = 0;

			auto len = GetLength();
			for (unsigned int i = 0; i < len; ++i)
				hash = ((hash << 5) + hash) + CharAt(i);

		}

	private:
		String* source;
		unsigned int hash;
		unsigned int begin;
		unsigned int end;
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
