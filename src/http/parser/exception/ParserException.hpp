// ParserException.hpp
#ifndef HTTP_PARSER_EXCEPTION_PARSEREXCEPTION_HPP
#define HTTP_PARSER_EXCEPTION_PARSEREXCEPTION_HPP

#include <stdexcept>
#include <string>

#include "../../Enums.hpp"

namespace http {
	class ParserException : public std::exception {
		private:
			std::string _message;
			http::StatusCode::Value _statusCode;

		public:
			ParserException(const std::string& message, http::StatusCode::Value statusCode) :
				_message(message), _statusCode(statusCode) {}
			virtual ~ParserException() throw() {}

			virtual const char* what() const throw() {
				return _message.c_str();
			}

			http::StatusCode::Value getStatusCode() const {
				return _statusCode;
			}
	};
}  // namespace http

#endif
