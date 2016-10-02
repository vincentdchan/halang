#pragma once
#include <ostream>
#include <functional>
#include <vector>
#include "halang.h"
#include "object.h"

namespace halang
{

	class String : Object
	{
	protected:
		// 00 for simple string
		// 01 for rope string
		// 10 for slice string
		unsigned int stringType : 2;

	public:
		static String* Concat(const String&, const String&);
		static String* Slice(const String&, unsigned int begin, unsigned int end);

		virtual Object* GetPrototype() const override { return nullptr; }

		virtual unsigned int Getlength() const = 0;
		virtual unsigned int GetHash() const = 0;
		virtual char16_t CharAt(unsigned int) const = 0;

		virtual bool operator==(const String& _Str) const = 0;

		inline bool isSimpleString() const { return stringType == 0; }
		inline bool isRopeString() const { return stringType == 1; }
		inline bool isSliceString() const { return stringType == 2; }
		
	};

	static bool operator==(const String& str1, const String& str2)
	{
		return str1.GetHash() == str2.GetHash();
	}

	class SimpleString : String
	{
	public:

		virtual unsigned int GetLength() const
		{
			return length;
		}

		virtual unsigned int GetHash() const
		{
			return hash;
		}

	private:
		char16_t* array;
		unsigned int hash;
		unsigned int length;
		unsigned int capacity;
	};

	class RopeString
	{

	};

	class SliceString
	{
	public:

		virtual unsigned int Getlength() const
		{
			return end - begin;
		}

		virtual unsigned int GetHash() const
		{
			return hash;
		}

		virtual char16_t CharAt(unsigned int index) const
		{
			return source->CharAt(this->begin + index);
		}

	private:
		String* source;
		unsigned int hash;
		unsigned int begin;
		unsigned int end;
	};

	class IString // ImmutableString
	{
	public:
		static unsigned int calculateHash(const char*);

		IString();
		IString(const char*);
		IString(const IString&);
		IString(IString&&);
		IString(const std::string&);
		IString& operator=(IString _is);

		IString operator+(IString _is) const;
		IString operator+(const char*) const;

		struct ReferData;
		std::string getStdString() const;
		inline unsigned int getHash() const { return _hash; }
		inline unsigned int getLength() const { return _length; }
		const char * c_str() { return _str; }

		friend class String;
		friend bool operator==(const IString& _s1, const IString& _s2);
		friend std::ostream& operator<<(std::ostream&, const IString& _Str);
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
