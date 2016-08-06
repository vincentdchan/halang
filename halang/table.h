#pragma once
#include "halang.h"
#include "object.h"
#include "string.h"
#include <unordered_map>
#include <vector>

namespace halang
{

	class Table : GCObject
	{
	public:
		Object& operator[](unsigned int _id) { return _number_map[_id]; }
		bool find(unsigned int _id) { return _number_map.find(_id) != _number_map.end(); }
	private:
		std::unordered_map<unsigned int, Object> _number_map;
		std::vector<Object> _vector;
	};

};
