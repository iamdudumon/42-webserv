// response.hpp
#ifndef HANDLER_UTILS_RESPONSE_HPP
#define HANDLER_UTILS_RESPONSE_HPP

#include <string>

#include "../../http/Enums.hpp"
#include "../../http/model/Packet.hpp"
#include "../../utils/str_utils.hpp"

namespace handler {
	namespace utils {
		inline http::Packet makePlainResponse(http::StatusCode::Value status,
											  const std::string& body,
											  const std::string& contentType) {
			http::StatusLine statusLine = {"HTTP/1.1", status,
										   http::StatusCode::to_reasonPhrase(status)};
			http::Packet response(statusLine, http::Header(), http::Body());

			response.addHeader("Content-Type", contentType);
			if (!body.empty()) response.appendBody(body.c_str(), body.size());
			return response;
		}

		// CGI 응답 생성
		inline std::string makeCgiResponse(const std::string& cgiOutput) {
			return "HTTP/1.1 200 OK\r\n" + cgiOutput;
		}
		inline std::string makeCgiErrorResponse(http::StatusCode::Value status) {
			std::string statusLine = "HTTP/1.1 " + int_tostr(status) + " " +
									 http::StatusCode::to_reasonPhrase(status) + "\r\n";
			return statusLine +
				   "\r\n"
				   "Content-Type: text/plain\r\n"
				   "Content-Length: 9\r\n"
				   "\r\n"
				   "CGI Error";
		}
	}  // namespace utils
}  // namespace handler

#endif
