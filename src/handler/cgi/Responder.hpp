// Responder.hpp
#ifndef HANDLER_CGI_RESPONDER_HPP
#define HANDLER_CGI_RESPONDER_HPP

#include <string>

#include "../../http/Enums.hpp"

namespace handler {
	namespace cgi {
		class Responder {
			private:
				struct CgiOutput {
						std::string httpHeader;
						std::string body;
						http::StatusCode::Value statusCode;
						std::string reasonPhrase;
						enum error {
							NONE,
							INVALID_FORMAT,
						} error;
						CgiOutput() :
							httpHeader(""),
							body(""),
							statusCode(http::StatusCode::UnknownStatus),
							reasonPhrase(""),
							error(NONE) {}
				};

				Responder() {}
				~Responder() {}
				static CgiOutput parseCgiOutput(const std::string& cgiResult);

			public:
				static std::string makeCgiResponse(const std::string&, bool keepAlive);
		};
	}  // namespace cgi
}  // namespace handler

#endif
