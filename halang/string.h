#pragma once
#include <ostream>
#include <functional>
#include <vector>
#include "halang.h"

namespace halang
{

	class IString // ImmutableString
	{
	public:
		static unsigned int calculateHash(const char*);

		IString();
		IString(const char*);
		IString(const IString&);
		IString(const std::string&);
		IString operator=(IString _is);

		IString operator+(IString _is);
		IString operator+(const char*);

		friend bool operator==(const IString& _s1, const IString& _s2);
		struct ReferData;
		friend class String;
		friend std::ostream& operator<<(std::ostream&, const IString& _Str);
		std::string getStdString();
		inline unsigned int getHash() { return _hash; }
		inline unsigned int getLength() { return _length; }
		char* c_str() { return _str; }
		~IString();
	private:
		ReferData* _ref_data;
		char* _str;
		unsigned int _hash;
		unsigned int _length;
	};

	struct IString::ReferData
	{
		ReferData(unsigned int _cnt = 1) : count(_cnt)
		{}
		unsigned int count;
	};

	inline std::ostream& operator<<(std::ostream& _ost, const IString& _Str)
	{
		_ost << _Str._str;
		return _ost;
	}

	inline bool operator==(const IString& _s1, const IString& _s2)
	{
		return (_s1._length == _s2._length) && (_s1._hash == _s2._hash);
	}

};

namespace std
{
	template<>
	struct hash<halang::IString>
	{
		unsigned int operator()(halang::IString _Str)
		{
			return _Str.getHash();
		}
	};
};
