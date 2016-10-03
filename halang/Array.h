#pragma once
#include "halang.h"
#include "object.h"
#include "context.h"
#include <cinttypes>

namespace halang
{

	class Array : public Object
	{
	public:
		typedef std::uint32_t size_type;
		typedef Object* DataType;

		static const size_type DEFAULT_CAPACITY = 32; // 2 ^ 5

		friend class GC;

	protected:

		/// <summary>
		/// Alloc some spaces to store pointers to the object
		/// </summary>
		Array(size_type _size = DEFAULT_CAPACITY)
		{
			typeId = TypeId::Array;
			base = Context::GetGc()->NewObjectPointerArray(_size);
			capacity = _size;
			length = 0;
		}

	public:

		static Array* Concat(Array*, Array*);
		static Array* Slice(Array*, unsigned int begin, unsigned int end);

		DataType& operator[](unsigned int);
		DataType& At(unsigned int);
		void Push(DataType data);
		DataType Pop();
		size_type GetLength();

	private:

		void realloc(size_type);

		DataType* base;
		size_type capacity;
		size_type length;

	};

	Array* Array::Concat(Array* arr1, Array* arr2)
	{
		size_type new_len = arr1->GetLength() + arr2->GetLength();
		auto new_arr = Context::GetGc()->New<Array>(new_len);

		for (size_type i = 0; i < arr1->GetLength(); ++i)
			new_arr[i] = arr1[i];

		auto arr1_len = arr1->GetLength();

		for (size_type i = 0; i < arr2->GetLength(); ++i)
			new_arr[arr1_len + i] = arr2[i];

		return new_arr;
	}

	Array* Array::Slice(Array* arr, unsigned int begin, unsigned int end)
	{
		size_type len = end - begin;
		auto new_arr = Context::GetGc()->New<Array>(len);

		for (unsigned int i = 0; i < len; ++i)
			new_arr[i] = arr[begin + i];

		return new_arr;
	}

	Array::DataType& Array::operator[](unsigned int index)
	{
		return At(index);
	}

	Array::DataType& Array::At(unsigned int index)
	{
		if (index > length)
			throw std::runtime_error("index out of range");
		return base[index];
	}

	void Array::Push(DataType data)
	{
		if (length == capacity)
			realloc(capacity * 2);
		base[length++] = data;
	}

	Array::DataType Array::Pop()
	{
		DataType last = base[--length];

		if (length < capacity / 4)
			realloc(capacity / 2);

		return last;
	}

	Array::size_type Array::GetLength()
	{
		return length;
	}

	void Array::realloc(size_type size)
	{
		auto new_space = Context::GetGc()->NewObjectPointerArray(size);
		for (size_type i = 0; i < length; ++i)
			new_space[i] = base[i];
		capacity = size;
		base = new_space;
	}

}
