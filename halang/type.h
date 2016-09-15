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

		HA_TYPE type;

		Type() :
			type(Object)
		{}
		Type(const Type& t):
			type(t.type)
		{}
		Type(HA_TYPE t):
			type(t)
		{}

		inline Type& operator=(const Type &t)
		{
			type = t.type;
			return *this;
		}

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
