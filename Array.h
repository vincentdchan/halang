#pragma once
#include "halang.h"
#include "object.h"
#include <vector>

namespace halang
{

	class Array : 
		public GCObject, private std::vector<Value>
	{

	public:

		friend class GC;
		friend class StackVM;

		typedef std::vector<Value>::size_type size_type;

	protected:

		Array()
		{

		}

		Array(unsigned int i):
			std::vector<Value>(i)
		{
		}

	public:

		virtual Value toValue() override
		{
			return Value(this, TypeId::Array);
		}

		void Push(Value v)
		{
			std::vector<Value>::push_back(v);
		}

		Value Pop()
		{
			return std::vector<Value>::operator[](std::vector<Value>::size() - 1);
		}

		inline size_type GetLength() const
		{
			return std::vector<Value>::size();
		}

		inline Value At(unsigned int index) const
		{
			if (index >= GetLength())
				throw std::runtime_error("<Array>index out of range");
			return std::vector<Value>::at(index);
		}

		inline void Set(unsigned int index, Value _v)
		{
			if (index >= GetLength())
				throw std::runtime_error("<Array>index out of range");
			std::vector<Value>::operator[](index) = _v;
		}

		inline void Resize(unsigned int _size)
		{
			std::vector<Value>::resize(_size);
		}

		virtual void Mark() override
		{
			marked = true;
			for (auto i = begin(); i != end(); ++i)
				if (i->isGCObject())
				{
					i->value.gc->Mark();
				}
		}

		Value& operator[](unsigned int i)
		{
			return std::vector<Value>::operator[](i);
		}

	};

}
