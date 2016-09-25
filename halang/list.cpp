#include "list.h"

namespace halang
{

	List::List()
	{
		array = new Object[GROUP_LENGTH];
		length = 0;

		array_capacity = GROUP_LENGTH;
	}

	std::size_t List::GetLength()
	{
		return length;
	}

	void List::Push(Object obj)
	{
		if (length < array_capacity)
		{
			array[length++] = obj;
		}
	}

	void List::SetValue(std::size_t index, Object obj)
	{
		if (!enableRope)
		{
			if (index >= length)
				throw std::out_of_range("Index out of range");
			array[index] = obj;
		}
	}

	void List::RemoveAt(std::size_t index)
	{
		if (!enableRope)
		{
			std::size_t i = index;
			while (i < length) array[i] = array[i + 1];
			length--;
		}
	}

	void List::Concat(const List& list)
	{

	}

	List::~List()
	{
		if (!enableRope)
		{
			delete[] array;
		}
	}

}