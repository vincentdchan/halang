#pragma once
#include <iostream>
#include <tuple>
#include <list>
#include <string>
#include <codecvt>
#include <locale>
#include <memory>
#include <utility>
#include "token.h"

namespace halang
{

	namespace utils
	{

		/// <summary>
		/// A helper class to help class to help collect pointer
		/// that create on the process and release them when the
		/// class is released.
		/// </summary>
		template<typename _BaseType>
		class PointerContainer
		{
		private:
			std::list<_BaseType*> _node_list;
		protected:

			/// <summary>
			/// record all the pointers of Node value.
			/// It can be clear when the parser is destructing.
			///
			/// make_node is similar to make_unqiue.
			/// </summary>
			/// <returns>The pointer of the object.</returns>
			template<typename _Ty, typename... _Types>
			_Ty* make_object(_Types&&... args)
			{
				_Ty* _node = new _Ty(std::forward<_Types>(args)...);
				_node_list.push_back(_node);
				return _node;
			}

		public:

			virtual ~PointerContainer()
			{
				for (auto i = _node_list.begin(); i != _node_list.end(); ++i)
					delete *i;
			}

		};

		/// <summary>
		/// A helper class to help record message of
		/// errors and warnings.
		/// </summary>
		template<typename _MsgType = std::string>
		class _MessageContainer
		{
		public:
			enum struct FLAG
			{
				NORMAL,
				WARNING,
				ERROR
			};

			// typedef std::tuple<_MsgType, Location, FLAG> MESSAGE;

			struct Message
			{
				Message(_MsgType _msg, Location _loc, FLAG _flag):
					msg(_msg), loc(_loc), flag(_flag)
				{}

				_MsgType msg;
				Location loc;
				FLAG flag;
			};

			_MessageContainer() : _hasError(false) {}
			_MessageContainer(const _MessageContainer& _con) :
				_messages(_con._messages), _hasError(_con._hasError)
			{}
			_MessageContainer(_MessageContainer&& _con) :
				_messages(std::move(_con._messages)), _hasError(_con._hasError)
			{}

			virtual ~_MessageContainer() {}

			void ReportMessage(const _MsgType& _content, Location _loc, FLAG _mt)
			{
				_messages.push_back(Message(_content, _loc, _mt));
			}

			/// <summary>
			/// Add a error message to the message container.
			/// </summary>
			void ReportError(const _MsgType& _content, Location _loc = Location())
			{
				_hasError = true;
				_messages.push_back(Message(_content, _loc, FLAG::ERROR));
			}

			/// <summary>
			/// Add a warining message to the message container.
			/// </summary>
			void ReportWarning(const _MsgType& _content, Location _loc = Location())
			{
				_messages.push_back(Message(_content, _loc, FLAG::WARNING));
			}

			/// <summary>
			/// Add a normal message to the message container.
			/// </summary>
			void ReportNormal(const _MsgType& _content, Location _loc = Location())
			{
				_messages.push_back(Message(_content, _loc, FLAG::NORMAL));
			}

			/// <summary>
			/// get the list of the message
			/// </summary>
			/// <returns>return the list of message</returns>
			const std::list<Message>& getMessages()
			{
				return _messages;
			}

			bool hasError()
			{
				return _hasError;
			}

			static std::ostream& OutputMsg(std::ostream& _os,
				const typename _MessageContainer<_MsgType>::Message& _msg)
			{
				switch (_msg.flag)
				{
				case _MessageContainer<_MsgType>::FLAG::NORMAL:
					_os << "Normal: "; break;
				case _MessageContainer<_MsgType>::FLAG::WARNING:
					_os << "Warnging: "; break;
				case _MessageContainer<_MsgType>::FLAG::ERROR:
					_os << "Error: "; break;
				}
				if (_msg.loc.line > -1)
				{
					_os << "line " << _msg.loc.line;
					if (_msg.loc.column > -1)
						_os << " Col " << _msg.loc.column << " : ";
				}
				_os << _msg.msg << std::endl;
				return _os;
			}
		private:
			std::list<Message> _messages;
			bool _hasError;
		};

		// why should I use 'inline' ?
		// http://stackoverflow.com/questions/6964819/function-already-defined-error-in-c

#if _MSC_VER == 1900

		static std::string UTF16_to_UTF8(std::u16string utf16_string)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
			auto p = reinterpret_cast<const int16_t *>(utf16_string.data());
			return convert.to_bytes(p, p + utf16_string.size());
		}


#else

		static std::string utf16_to_utf8(std::u16string utf16_string)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
			return convert.to_bytes(utf16_string);
		}

#endif

		static std::u16string utf8_to_utf16(const std::string& utf8)
		{
			std::vector<unsigned long> _unicode;
			size_t i = 0;
			while (i < utf8.size())
			{
				unsigned long uni;
				size_t todo;
				bool error = false;
				unsigned char ch = utf8[i++];
				if (ch <= 0x7F)
				{
					uni = ch;
					todo = 0;
				}
				else if (ch <= 0xBF)
				{
					throw std::logic_error("not a UTF-8 string");
				}
				else if (ch <= 0xDF)
				{
					uni = ch & 0x1F;
					todo = 1;
				}
				else if (ch <= 0xEF)
				{
					uni = ch & 0x0F;
					todo = 2;
				}
				else if (ch <= 0xF7)
				{
					uni = ch & 0x07;
					todo = 3;
				}
				else
				{
					throw std::logic_error("not a UTF-8 string");
				}
				for (size_t j = 0; j < todo; ++j)
				{
					if (i == utf8.size())
						throw std::logic_error("not a UTF-8 string");
					unsigned char ch = utf8[i++];
					if (ch < 0x80 || ch > 0xBF)
						throw std::logic_error("not a UTF-8 string");
					uni <<= 6;
					uni += ch & 0x3F;
				}
				if (uni >= 0xD800 && uni <= 0xDFFF)
					throw std::logic_error("not a UTF-8 string");
				if (uni > 0x10FFFF)
					throw std::logic_error("not a UTF-8 string");
				_unicode.push_back(uni);
			}
			std::u16string utf16;
			for (size_t i = 0; i < _unicode.size(); ++i)
			{
				unsigned long uni = _unicode[i];
				if (uni <= 0xFFFF)
				{
					utf16 += (char16_t)uni;
				}
				else
				{
					uni -= 0x10000;
					utf16 += (char16_t)((uni >> 10) + 0xD800);
					utf16 += (char16_t)((uni & 0x3FF) + 0xDC00);
				}
			}
			return utf16;
		}

	};

};

namespace std
{
	template<typename _MsgType>
	inline std::ostream& operator<<(std::ostream& _os,
		const typename halang::utils::_MessageContainer<_MsgType>::Message& _msg)
		{
			switch (_msg.flag)
			{
			case _MessageContainer<_MsgType>::FLAG::NORMAL:
				_os << "Normal: "; break;
			case _MessageContainer<_MsgType>::FLAG::WARNING:
				_os << "Warnging: "; break;
			case _MessageContainer<_MsgType>::FLAG::ERROR:
				_os << "Error: "; break;
			}
			if (_msg.loc.line > -1)
			{
				_os << "line " << _msg.loc.line;
				if (_location.column > -1)
					_os << " Col " << _msg.loc.column << " : ";
			}
			_os << _msg.msg << std::endl;
			return _os;
		}

};