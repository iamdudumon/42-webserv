#include "ResponseFactory.hpp"

#include <sstream>

#include "../handler/exception/Exception.hpp"
#include "../utils/file_utils.hpp"
#include "../utils/str_utils.hpp"

using namespace http;

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

	bool loadPageContent(const std::string& path, std::string& out) {
		if (path.empty()) return false;
		FileInfo file = readFile(path.c_str());
		if (file.error != FileInfo::NONE) return false;
		out = file.content;
		return true;
	}
}

ResponseFactory::ResponseFactory() {}
ResponseFactory::~ResponseFactory() {}

Packet ResponseFactory::create(StatusCode::Value status, const std::string& body,
							   const std::string& contentType) {
	StatusLine statusLine = {"HTTP/1.1", status, StatusCode::to_reasonPhrase(status)};
	Packet response(statusLine, Header(), Body());

	if (!contentType.empty()) response.addHeader("Content-Type", contentType);
	if (!body.empty()) response.appendBody(body.c_str(), body.size());
	return response;
}

Packet ResponseFactory::createFileResponse(StatusCode::Value status, const std::string& body,
										   const std::string& contentType) {
	std::string finalContentType = contentType;
	if (finalContentType.empty()) finalContentType = "application/octet-stream";
	return create(status, body, finalContentType);
}

Packet ResponseFactory::createError(StatusCode::Value status, const config::Config* config,
									const std::string& fallbackBody,
									const std::string& fallbackContentType) {
	std::string body = fallbackBody;
	std::string contentType = fallbackContentType;
	bool loaded = false;

	if (config) {
		const std::map<int, std::string>& errorPages = config->getErrorPages();
		std::map<int, std::string>::const_iterator it = errorPages.find(static_cast<int>(status));
		if (it != errorPages.end() && loadPageContent(it->second, body)) {
			contentType = "text/html";
			loaded = true;
		}
	}
	if (!loaded) {
		std::string reason = StatusCode::to_reasonPhrase(status);
		std::string message = fallbackBody.empty() ? "요청을 처리할 수 없습니다." : fallbackBody;
		body = std::string("<!DOCTYPE html><html><head><meta charset=\"utf-8\">") + "<title>" +
			   int_tostr(status) + " " + reason + "</title>" +
			   "<style>"
			   "body{font-family:Arial,'Noto Sans KR',sans-serif;background:#f6f7fb;"
			   "color:#333;display:flex;align-items:center;justify-content:center;"
			   "min-height:100vh;margin:0;}"
			   ".box{background:#fff;border-radius:18px;padding:48px 56px;text-align:center;"
			   "box-shadow:0 12px 36px rgba(0,0,0,0.12);max-width:520px;}"
			   ".code{font-size:4rem;font-weight:900;margin-bottom:0.4em;color:#5b86e5;}"
			   ".title{font-size:1.8rem;font-weight:700;margin-bottom:0.6em;}"
			   ".msg{font-size:1.05rem;line-height:1.6;color:#555;word-break:keep-all;}"
			   "</style></head><body><div class=\"box\">" +
			   "<div class=\"code\">" + int_tostr(status) + "</div>" + "<div class=\"title\">" +
			   reason + "</div>" + "<div class=\"msg\">" + message + "</div>" +
			   "</div></body></html>";
		contentType = "text/html";
	} else if (contentType.empty())
		contentType = "text/html";

	return create(status, body, contentType);
}

Packet ResponseFactory::createRedirect(StatusCode::Value status, const std::string& location) {
	Packet response = create(status);
	response.addHeader("Location", location);
	return response;
}

std::string ResponseFactory::createCgiResponse(const std::string& cgiResult, bool keepAlive) {
	CgiOutput cgiOutput = parseCgiOutput(cgiResult);
	StatusCode::Value statusCode = cgiOutput.statusCode;
	std::string reasonPhrase = cgiOutput.reasonPhrase;

	if (cgiOutput.error == CgiOutput::INVALID_FORMAT) {
		throw handler::Exception("CGI Invalid Format");
	}
	if (reasonPhrase.empty()) reasonPhrase = StatusCode::to_reasonPhrase(statusCode);

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
