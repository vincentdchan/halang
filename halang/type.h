#pragma once

namespace halang
{
	class Type
	{
	public:
		enum HA_TYPE
		{
			INT, NUMBER, BOOL, Object
		};

		HA_TYPE type = Object;

		Type()
		{}
		Type(Type& t):
			type(t.type)
		{}

		Type(HA_TYPE t):
			type(t)
		{}


		inline bool operator==(Type rhs) const
		{
			return this->type == rhs.type;
		}

		inline bool operator!=(Type rhs) const
		{
			return this->type != rhs.type;
		}
			 
	};
}
