#pragma once
#include "halang.h"
#include "object.h"
#include "string.h"
#include <exception>
#include <unordered_map>
#include <functional>

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
				auto str = halang::IString(*obj.value.str);
				auto h = hash<halang::IString>();
				return h(str);
			}
			case halang::Object::TYPE::NUMBER:
			{
				auto h = hash<halang::TNumber>();
				return h(obj.value.number);
			}
			case halang::Object::TYPE::SMALL_INT :
			{
				auto h = hash<halang::TSmallInt>();
				return h(obj.value.si);
			}
			default:
				throw std::runtime_error("Error on hash to a unvalid type.");
			}
		}
	};

}

namespace halang
{
	class Dict : GCObject
	{
	public:

		static const int DEFAULT_ARRAY_CAPACITY = 20;

		Dict();
		Dict(const Dict&);
		virtual ~Dict();

		void SetValue(Object, Object);
		Object GetValue(Object);

	private:

		int array_len;
		Object* arrays;
		std::unordered_map<Object, Object> maps;

	};

}

