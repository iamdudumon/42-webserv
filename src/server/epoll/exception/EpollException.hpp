// EpollException.hpp
#ifndef SERVER_EPOLL_EXCEPTION_HPP
#define SERVER_EPOLL_EXCEPTION_HPP

#include <exception>
#include <string>

namespace server {
	class EpollException : public std::exception {
		private:
			std::string _message;

		public:
			explicit EpollException(const std::string& message) :
				_message("[Error] Epoll " + message + " Error") {}
			virtual ~EpollException() throw() {}

			virtual const char* what() const throw() {
				return _message.c_str();
			}
	};
}  // namespace server

#endif
