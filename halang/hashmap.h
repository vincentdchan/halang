#pragma once
#include "halang.h"
#include "object.h"
#include "string.h"
#include <unordered_map>
#include <exception>

namespace std
{

	template<>
	struct hash<halang::Object>
	{

		std::size_t operator()(const halang::Object& obj) const
		{
			switch (obj.type)
			{
			case halang::Object::TYPE::STRING:
			{
				auto str = reinterpret_cast<halang::IString*>(obj.value.str);
				return std::hash<halang::IString>()(*str);
			}
			case halang::Object::TYPE::SMALL_INT:
			{
				auto i = obj.value.si;
				return std::hash<halang::TSmallInt>()(i);
			}
			case halang::Object::TYPE::NUMBER:
			{
				auto num = obj.value.number;
				return std::hash<halang::TNumber>()(num);
			}
			default:
				throw std::runtime_error("You can only calculate hash for ");
			}
		}

	};

}

namespace halang
{

	class HashMap : public GCObject
	{

		HashMap();
		HashMap(const HashMap&);
		HashMap(const std::unordered_map<Object, Object>&);

		void SetValue(Object, Object);
		Object GetValue(Object);

	private:

		std::unordered_map<Object, Object> inner_map;

	};

}
