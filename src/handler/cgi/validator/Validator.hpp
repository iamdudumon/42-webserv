// Validator.hpp
#ifndef HANDLER_CGI_VALIDATOR_HPP
#define HANDLER_CGI_VALIDATOR_HPP

#include <string>

#include "../../../http/Enums.hpp"
#include "../../../utils/str_utils.hpp"

namespace handler {
	namespace cgi {
		struct CgiOutput {
				std::string httpHeader;
				std::string body;
				http::StatusCode::Value statusCode;
				enum error {
					NONE,
					INVALID_FORMAT,
				} error;
		};

		class OutputValidator {
			private:
				OutputValidator() {}
				~OutputValidator() {}
			public:
				static CgiOutput validateCgiOutput(const std::string&);
		};
	}  // namespace cgi
}  // namespace handler

#endif