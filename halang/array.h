#pragma once
#include <exception>
#include <vector>
#include "svm.h"
#include "halang.h"
#include "object.h"

namespace halang
{

	/// <summary>
	/// List is a data structure for Halang
	/// It's implemented by Vector in stl
	///
	/// </summary>
	class Array : public GCObject
	{
	public:

		static Array* Concat(StackVM* vm, Array* a, Array* b);
		static Array* Slice(StackVM* vm, Array*, 
			std::size_t begin, std::size_t end);
			
		Array();
		Array(const std::vector<Object>&);
		Array(const Array&);

		inline std::size_t GetLength()
		{
			return inner_vec.size();
		}

		inline Object At(std::size_t index)
		{
			if (index >= GetLength())
			{
				throw std::out_of_range("index out of range");
			}
			return inner_vec[index];
		}

		void Push(Object obj);
		void SetValue(std::size_t index, Object);
		Object Pop();

		void RemoveAt(std::size_t);

		// concat
		// slice

		virtual ~Array();

	private:

		std::vector<Object> inner_vec;

	};

}
