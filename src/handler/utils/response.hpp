// response.hpp
#ifndef HANDLER_UTILS_RESPONSE_HPP
#define HANDLER_UTILS_RESPONSE_HPP

#include <string>

#include "../../config/model/Config.hpp"
#include "../../handler/exception/Exception.hpp"
#include "../../http/Enums.hpp"
#include "../../http/model/Packet.hpp"
#include "../../http/serializer/Serializer.hpp"
#include "../../utils/file_utils.hpp"
#include "../../utils/str_utils.hpp"

namespace handler {
	namespace utils {
		inline bool loadPageContent(const std::string& path, std::string& out) {
			if (path.empty()) return false;
			FileInfo file = readFile(path.c_str());
			if (file.error != FileInfo::NONE) return false;
			out = file.content;
			return true;
		}

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

		inline http::Packet makeErrorPacket(
			http::StatusCode::Value status, const config::Config* config,
			const std::string& fallbackBody = std::string(),
			const std::string& fallbackContentType = std::string()) {
			std::string body = fallbackBody;
			std::string contentType = fallbackContentType;
			bool loaded = false;

			if (config) {
				const std::map<int, std::string>& errorPages = config->getErrorPages();
				std::map<int, std::string>::const_iterator it =
					errorPages.find(static_cast<int>(status));
				if (it != errorPages.end() && loadPageContent(it->second, body)) {
					contentType = "text/html";
					loaded = true;
				}
			}

			if (!loaded) {
				std::string defaultPath = "var/www/errors/" + int_tostr(status) + ".html";
				if (loadPageContent(defaultPath, body)) {
					contentType = "text/html";
					loaded = true;
				}
			}

			if (!loaded) {
				if (body.empty()) body = http::StatusCode::to_reasonPhrase(status);
				if (contentType.empty()) contentType = "text/plain";
			} else if (contentType.empty()) {
				contentType = "text/html";
			}

			http::StatusLine statusLine = {"HTTP/1.1", status,
										   http::StatusCode::to_reasonPhrase(status)};
			http::Packet response(statusLine, http::Header(), http::Body());
			if (!contentType.empty()) response.addHeader("Content-Type", contentType);
			if (!body.empty()) response.appendBody(body.c_str(), body.size());
			return response;
		}

		inline std::string makeErrorResponse(
			http::StatusCode::Value status, const config::Config* config = NULL,
			const std::string& fallbackBody = std::string(),
			const std::string& fallbackContentType = std::string()) {
			http::Packet packet =
				makeErrorPacket(status, config, fallbackBody, fallbackContentType);
			return http::Serializer::serialize(packet);
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
