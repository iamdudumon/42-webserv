// Exception.hpp
#ifndef BUILDER_EXCEPTION_HPP
#define BUILDER_EXCEPTION_HPP

#include <exception>
#include <string>

namespace handler {
	namespace builder {
		class Exception : public std::exception {
			private:
				std::string _message;

			public:
				Exception() {}
				explicit Exception(const std::string& message) : _message(message) {}
				virtual ~Exception() throw() {}

				virtual const char* what() const throw() {
					return _message.c_str();
				}
		};
	}  // namespace builder
}  // namespace handler

#endif
