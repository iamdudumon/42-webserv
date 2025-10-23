// response.hpp
#ifndef HANDLER_UTILS_RESPONSE_HPP
#define HANDLER_UTILS_RESPONSE_HPP

#include <string>

#include "../../handler/exception/Exception.hpp"
#include "../../http/Enums.hpp"
#include "../../http/model/Packet.hpp"
#include "../../utils/file_utils.hpp"
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

		inline std::string makeErrorResponse(
			http::StatusCode::Value status = http::StatusCode::InternalServerError) {
			std::string statusLine = "HTTP/1.1 " + int_tostr(status) + " " +
									 http::StatusCode::to_reasonPhrase(status) + "\r\n";
			std::string path = "var/www/errors/" + int_tostr(status) + ".html";
			FileInfo body = readFile(path.c_str());
			return statusLine + "Content-Type: text/html\r\n" +
				   "Content-Length: " + int_tostr(body.content.size()) + "\r\n" + "\r\n" +
				   body.content;
		}

		inline std::string makeCgiResponse(std::string& cgiOutput) {
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
				throw handler::Exception();
			}
			return statusLine + cgiOutput;
		}
	}  // namespace utils
}  // namespace handler

#endif
