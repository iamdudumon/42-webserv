// Exception.hpp
#ifndef SERVER_EXCEPTION_HPP
#define SERVER_EXCEPTION_HPP

#include <exception>
#include <string>

namespace server {
	class Exception : public std::exception {
		private:
			std::string _message;

		public:
			explicit Exception(const std::string& message) :
				_message("[Error] Server " + message + " Error") {}
			virtual ~Exception() throw() {}

			virtual const char* what() const throw() {
				return _message.c_str();
			}
	};
}  // namespace server

#endif
