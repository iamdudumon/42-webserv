// response.hpp
#ifndef HANDLER_UTILS_RESPONSE_HPP
#define HANDLER_UTILS_RESPONSE_HPP

#include <iostream>
using namespace std;

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

		inline std::string makeCgiResponse(
			std::string& cgiOutput,
			http::StatusCode::Value defaultStatusCode = http::StatusCode::OK) {
			size_t headerEnd = cgiOutput.find("\r\n\r\n");
			std::string httpHeader;
			if (headerEnd != std::string::npos) {
				httpHeader = cgiOutput.substr(0, headerEnd);
			}
			std::string statusLine;
			size_t statusPos = httpHeader.find("Status: ");
			if (statusPos != std::string::npos) {
				size_t lineEnd = httpHeader.find("\r\n", statusPos);
				std::string statusStr = httpHeader.substr(statusPos + 8, lineEnd - (statusPos + 8));
				http::StatusCode::Value statusCode =
					static_cast<http::StatusCode::Value>(str_toint(statusStr));
				cgiOutput.erase(0, lineEnd + 2);
				statusLine = "HTTP/1.1 " + int_tostr(statusCode) + " " +
							 http::StatusCode::to_reasonPhrase(statusCode) + "\r\n";
			} else {
				statusLine = "HTTP/1.1 " + int_tostr(defaultStatusCode) + " " +
							 http::StatusCode::to_reasonPhrase(defaultStatusCode) + "\r\n";
			}
			return statusLine + cgiOutput;
		}
	}  // namespace utils
}  // namespace handler

#endif
