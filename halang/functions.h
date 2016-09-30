#pragma once
#include <vector>
#include "function.h"
#include "halang.h"
#include "object.h"
#include "svm.h"

namespace halang
{

	static void ConcatTwoArray(FunctionCallbackInfo& args)
	{
		auto arg1 = args[0];
		auto arg2 = args[1];
		if (!arg1.isArray() || !arg2.isArray())
			throw std::runtime_error("You must concat two list.");

		auto array1 = Object::PtCast<Array>(arg1);
		auto array2 = Object::PtCast<Array>(arg2);

		auto array3 = Array::Concat(args.GetIsolate()->GetVM(), array1, array2);
		args.SetReturnObject(Object(array3, Object::TYPE::ARRAY));
	}

	static void SliceTwoArray(FunctionCallbackInfo& args)
	{
		auto arg1 = args[0];
		auto arg2 = args[1];
		auto arg3 = args[2];
		if (!arg1.isArray() || !arg2.isSmallInt() || !arg3.isSmallInt())
			throw std::runtime_error("You must slice a array with two ints.");

		auto arr = Object::PtCast<Array>(arg1);
		auto begin = arg2.value.si;
		auto end = arg3.value.si;

		auto new_arr = Array::Slice(args.GetIsolate()->GetVM(), arr, begin, end);
		args.SetReturnObject(Object(new_arr, Object::TYPE::ARRAY));
	}

}
