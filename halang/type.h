#pragma once

namespace halang
{
	class Type
	{
	public:
		enum HA_TYPE
		{
			INT, NUMBER, BOOL, STRING, OBJECT, UNDEFINED
		};

		HA_TYPE type;

		Type() :
			type(UNDEFINED)
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

		inline void setType(HA_TYPE t)
		{
			this->type = t;
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
