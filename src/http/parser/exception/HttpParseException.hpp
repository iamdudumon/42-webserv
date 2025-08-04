// HttpParserException.hpp
#ifndef HTTP_PARSE_EXCEPTION_HPP
#define HTTP_PARSE_EXCEPTION_HPP

#include <stdexcept>
#include <string>

class HttpParseException : public std::exception {
	private:
		std::string				_message;
		HTTP::StatusCode::Value _statusCode;

	public:
		HttpParseException(const std::string&	   message,
						   HTTP::StatusCode::Value statusCode)
			: _message(message), _statusCode(statusCode) {}
		virtual ~HttpParseException() throw() {}

		virtual const char* what() const throw() { return _message.c_str(); }

		HTTP::StatusCode::Value getStatusCode() const { return _statusCode; }
};

#endif
