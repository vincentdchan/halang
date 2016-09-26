#include "array.h"

namespace halang
{

	Array* Array::Concat(StackVM *vm, Array* a, Array* b)
	{
		auto new_arr =vm->make_gcobject<Array>(*a);
		new_arr->inner_vec.insert(new_arr->inner_vec.end(), 
			b->inner_vec.begin(), b->inner_vec.end());
		return new_arr;
	}

	Array* Array::Slice(StackVM* vm, Array* arr,
		std::size_t begin, std::size_t end)
	{
		auto new_arr = vm->make_gcobject<Array>();

		for (auto i = begin; i != end; ++i)
			new_arr->Push(arr->At(i));

		return new_arr;
	}

	Array::Array()
	{}

	Array::Array(const std::vector<Object>& _l):
		inner_vec(_l)
	{}

	Array::Array(const Array& arr) :
		inner_vec(arr.inner_vec)
	{}

	void Array::Push(Object obj)
	{
		inner_vec.push_back(obj);
	}

	void Array::SetValue(std::size_t index, Object obj)
	{
		if (index >= GetLength())
			throw std::out_of_range("Index out of range");
		inner_vec[index] = obj;
	}

	void Array::RemoveAt(std::size_t index)
	{
		if (index >= GetLength())
			throw std::out_of_range("Index out of range");

		auto it = inner_vec.begin();
		std::advance(it, index);
		inner_vec.erase(it);
	}

	Object Array::Pop()
	{
		auto obj = inner_vec.back();
		inner_vec.pop_back();
		return obj;
	}


	Array::~Array()
	{
	}

}