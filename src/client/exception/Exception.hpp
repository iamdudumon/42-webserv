#ifndef CLIENT_EXCEPTION_HPP
#define CLIENT_EXCEPTION_HPP

#include <exception>
#include <string>

#include "../../http/Enums.hpp"

namespace client {
	class Exception : public std::exception {
		private:
			std::string _errorMessage;
			http::StatusCode::Value _errorCode;

		public:
			Exception(http::StatusCode::Value errorCode, const std::string& errorMessage) :
				_errorMessage(errorMessage), _errorCode(errorCode) {}
			virtual ~Exception() throw() {}

			virtual const char* what() const throw() {
				return "Client Exception";
			}
			http::StatusCode::Value getErrorCode() const {
				return _errorCode;
			}
			std::string getErrorMessage() const {
				return _errorMessage;
			}
	};
}  // namespace client

#endif
