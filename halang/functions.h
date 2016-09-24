#pragma once
#include <vector>
#include <functional>
#include "halang.h"
#include "object.h"
#include "svm.h"

namespace halang
{
	typedef std::vector<Object> ParamListType;
	typedef std::function<void(ParamListType*, StackVM*)> ExternFunction;

	namespace 
	{
	}

}
