#pragma once
#include <iostream>
#include <tuple>
#include <list>
#include <utility>
#include <string>
#include "token.h"

namespace halang
{

	namespace utils
	{

		/// <summary>
		/// A helper class to help record message of
		/// errors and warnings.
		/// </summary>
		class _MessageContainer
		{
		public:
			enum struct FLAG
			{
				NORMAL,
				WARNING,
				ERROR
			};
			typedef std::tuple<std::string, Location, FLAG> MESSAGE;

			_MessageContainer() : _hasError(false) {}
			_MessageContainer(const _MessageContainer& _con) :
				_messages(_con._messages), _hasError(_con._hasError)
			{}
			_MessageContainer(_MessageContainer&& _con) :
				_messages(std::move(_con._messages)), _hasError(_con._hasError)
			{}

			void ReportMessage(const std::string& _content, Location _loc, FLAG _mt)
			{
				_messages.push_back(std::make_tuple(std::move(_content), std::move(_loc), _mt));
			}

			/// <summary>
			/// Add a error message to the message container.
			/// </summary>
			void ReportError(const std::string& _content, Location _loc = Location())
			{
				_hasError = true;
				_messages.push_back(std::make_tuple(std::move(_content), std::move(_loc), FLAG::ERROR));
			}

			/// <summary>
			/// Add a warining message to the message container.
			/// </summary>
			void ReportWarning(const std::string& _content, Location _loc = Location())
			{
				_messages.push_back(std::make_tuple(std::move(_content), std::move(_loc), FLAG::WARNING));
			}

			/// <summary>
			/// Add a normal message to the message container.
			/// </summary>
			void ReportNormal(const std::string& _content, Location _loc = Location())
			{
				_messages.push_back(std::make_tuple(std::move(_content), std::move(_loc), FLAG::NORMAL));
			}

			/// <summary>
			/// get the list of the message
			/// </summary>
			/// <returns>return the list of message</returns>
			const std::list<MESSAGE>& getMessages()
			{
				return _messages;
			}
			bool hasError()
			{
				return _hasError;
			}
		private:
			std::list<MESSAGE> _messages;
			bool _hasError;
		};

		// why should I use 'inline' ?
		// http://stackoverflow.com/questions/6964819/function-already-defined-error-in-c

		inline std::ostream& operator<<(std::ostream& _os, _MessageContainer::MESSAGE _msg)
		{
			std::string _msg_content;
			Location _location;
			_MessageContainer::FLAG _msg_type;
			std::tie(_msg_content, _location, _msg_type) = _msg;
			std::string _result;
			switch (_msg_type)
			{
			case _MessageContainer::FLAG::NORMAL:
				_os << "Normal: "; break;
			case _MessageContainer::FLAG::WARNING:
				_os << "Warnging: "; break;
			case _MessageContainer::FLAG::ERROR:
				_os << "Error: "; break;
			}
			if (_location.line > -1)
			{
				_os << "line " << _location.line;
				if (_location.column > -1)
					_os << " Col " << _location.column << " : ";
			}
			_os << _msg_content << std::endl;
			return _os;
		}
	};

};
