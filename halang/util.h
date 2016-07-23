#pragma once
#include <iostream>
#include <tuple>
#include <list>
#include <utility>
#include "token.h"

namespace utils
{
	class _MessageContainer
	{
	public:
		enum struct MESSAGE_TYPE
		{
			NORMAL,
			WARNING,
			ERROR
		};
		typedef std::tuple<std::string, lex::Location, MESSAGE_TYPE> MESSAGE;

		_MessageContainer(): _hasError(false) {}
		_MessageContainer(const _MessageContainer& _con):
			_messages(_con._messages), _hasError(_con._hasError)
		{}
		_MessageContainer(_MessageContainer&& _con) :
			_messages(std::move(_con._messages)), _hasError(_con._hasError)
		{}

		void ReportMessage(const std::string& _content, lex::Location _loc, MESSAGE_TYPE _mt)
		{
			_messages.push_back(std::make_tuple(std::move(_content), std::move(_loc), _mt));
		}
		void ReportError(const std::string& _content, lex::Location _loc = lex::Location())
		{
			_hasError = true;
			_messages.push_back(std::make_tuple(std::move(_content), std::move(_loc), MESSAGE_TYPE::ERROR));
		}
		void ReportWarning(const std::string& _content, lex::Location _loc = lex::Location())
		{
			_messages.push_back(std::make_tuple(std::move(_content), std::move(_loc), MESSAGE_TYPE::WARNING));
		}
		void ReportNormal(const std::string& _content, lex::Location _loc = lex::Location())
		{
			_messages.push_back(std::make_tuple(std::move(_content), std::move(_loc), MESSAGE_TYPE::NORMAL));
		}
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
		lex::Location _location;
		_MessageContainer::MESSAGE_TYPE _msg_type;
		std::tie(_msg_content, _location, _msg_type) = _msg;
		std::string _result;
		switch (_msg_type)
		{
		case _MessageContainer::MESSAGE_TYPE::NORMAL:
			_os << "Normal: "; break;
		case _MessageContainer::MESSAGE_TYPE::WARNING:
			_os << "Warnging: "; break;
		case _MessageContainer::MESSAGE_TYPE::ERROR:
			_os << "Error: "; break;
		}
		if (_location.line > -1)
		{
			_os << "line " << _location.line;
			if (_location.begin > -1)
				_os << " Col " << _location.begin << " : ";
		}
		_os << _msg_content << std::endl;
		return _os;
	}
}
