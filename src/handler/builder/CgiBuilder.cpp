#include "CgiBuilder.hpp"

#include "../../http/Enums.hpp"
#include "../../utils/str_utils.hpp"
#include "../exception/Exception.hpp"

using namespace handler::builder;

namespace {
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

	CgiOutput parseCgiOutput(const std::string& cgiResult) {
		CgiOutput output;

		size_t headerEnd = cgiResult.find("\r\n\r\n");
		if (headerEnd == std::string::npos) {
			output.error = CgiOutput::INVALID_FORMAT;
			return output;
		}

		std::string httpHeader = cgiResult.substr(0, headerEnd);
		std::string body = cgiResult.substr(headerEnd + 4);

		size_t statusPos = httpHeader.find("Status: ");
		if (statusPos != 0) {
			output.error = CgiOutput::INVALID_FORMAT;
			return output;
		}

		size_t contentTypePos = httpHeader.find("\r\nContent-Type: ");
		if (contentTypePos == std::string::npos) {
			output.error = CgiOutput::INVALID_FORMAT;
			return output;
		}

		size_t lineEnd = httpHeader.find("\r\n", 0);
		std::string statusLine = trim(httpHeader.substr(8, lineEnd - 8));
		if (statusLine.empty()) {
			output.error = CgiOutput::INVALID_FORMAT;
			return output;
		}
		size_t spacePos = statusLine.find(' ');
		std::string statusCodeToken =
			spacePos == std::string::npos ? statusLine : statusLine.substr(0, spacePos);
		if (statusCodeToken.empty()) {
			output.error = CgiOutput::INVALID_FORMAT;
			return output;
		}
		for (size_t i = 0; i < statusCodeToken.size(); ++i) {
			if (!std::isdigit(static_cast<unsigned char>(statusCodeToken[i]))) {
				output.error = CgiOutput::INVALID_FORMAT;
				return output;
			}
		}
		std::string reason =
			spacePos == std::string::npos ? std::string() : trim(statusLine.substr(spacePos + 1));
		int statusInt = str_toint(statusCodeToken);
		if (statusInt < 100 || statusInt > 599) {
			output.error = CgiOutput::INVALID_FORMAT;
			return output;
		}

		output.statusCode = static_cast<http::StatusCode::Value>(statusInt);
		output.reasonPhrase = reason;
		output.httpHeader = httpHeader;
		output.body = body;
		output.error = CgiOutput::NONE;
		return output;
	}
}

std::string CgiBuilder::build(const std::string& cgiResult, bool keepAlive) const {
	CgiOutput cgiOutput = parseCgiOutput(cgiResult);
	http::StatusCode::Value statusCode = cgiOutput.statusCode;
	std::string reasonPhrase = cgiOutput.reasonPhrase;

	if (cgiOutput.error == CgiOutput::INVALID_FORMAT) {
		throw handler::Exception("CGI Invalid Format");
	}
	if (reasonPhrase.empty()) reasonPhrase = http::StatusCode::to_reasonPhrase(statusCode);

	std::string httpHeader = cgiOutput.httpHeader;
	std::string body = cgiOutput.body;
	httpHeader.erase(0, httpHeader.find("\r\n", 0) + 2);
	std::string statusLine = "HTTP/1.1 " + int_tostr(statusCode) + " " + reasonPhrase + "\r\n";
	httpHeader +=
		("\r\n"
		 "Content-Length: " +
		 int_tostr(body.size()) +
		 "\r\n"
		 "Server: webserv\r\n"
		 "Connection: " +
		 std::string(keepAlive ? "keep-alive" : "close") + "\r\n\r\n");
	return statusLine + httpHeader + body;
}
