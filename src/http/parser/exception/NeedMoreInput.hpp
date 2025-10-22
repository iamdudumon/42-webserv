// NeedMoreInput.hpp
#ifndef HTTP_PARSER_NEEDMOREINPUT_HPP
#define HTTP_PARSER_NEEDMOREINPUT_HPP

#include <exception>

namespace http {
	class NeedMoreInput : public std::exception {
		public:
			NeedMoreInput() {}
			virtual ~NeedMoreInput() throw() {}

			virtual const char* what() const throw() {
				return "Parser requires additional input";
			}
	};
}  // namespace http

#endif
