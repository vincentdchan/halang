#pragma once
#include <exception>
#include "halang.h"
#include "object.h"

namespace halang
{

	/// <summary>
	/// List is a data structure for Halang
	/// It's supposed to have hight performantce
	/// at index, indexing, delete, concat
	/// 
	/// When the length of the list is less than A
	/// 1. A array in memory
	/// 2. A Rope datasturcture
	///
	/// </summary>
	class List : GCObject
	{
	public:
		List();
		List(const List&) = delete;

		std::size_t GetLength();

		void Push(Object obj);
		void SetValue(std::size_t index, Object);
		Object Pop();

		void RemoveAt(std::size_t);
		void Concat(const List&);

		List Clone();

		virtual ~List();

	private:

		static const std::size_t GROUP_LENGTH = 20;

		std::size_t length;

		std::size_t array_capacity;
		Object* array = nullptr;

		bool enableRope = false;

	};

}
