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
