// Exception.hpp
#ifndef CONFIG_EXCEPTION_HPP
#define CONFIG_EXCEPTION_HPP

#include <exception>
#include <string>

namespace config {
	class Exception : public std::exception {
		private:
			std::string _message;

		public:
			explicit Exception(const std::string& message) : _message(message) {}
			virtual ~Exception() throw() {}

			virtual const char* what() const throw() {
				return _message.c_str();
			}
	};
}  // namespace config

#endif
