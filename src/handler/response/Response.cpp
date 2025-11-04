#include "Response.hpp"

using namespace handler;

http::Packet response::makePlainResponse(http::StatusCode::Value status, const std::string& body,
							   const std::string& contentType) {
	http::StatusLine statusLine = {"HTTP/1.1", status, http::StatusCode::to_reasonPhrase(status)};
	http::Packet response(statusLine, http::Header(), http::Body());

	response.addHeader("Content-Type", contentType);
	if (!body.empty()) response.appendBody(body.c_str(), body.size());
	return response;
}

http::Packet response::makeErrorResponse(http::StatusCode::Value status,
										 const config::Config* config,
										 const std::string& fallbackBody,
										 const std::string& fallbackContentType) {
	std::string body = fallbackBody;
	std::string contentType = fallbackContentType;
	bool loaded = false;

	if (config) {
		const std::map<int, std::string>& errorPages = config->getErrorPages();
		std::map<int, std::string>::const_iterator it = errorPages.find(static_cast<int>(status));
		if (it != errorPages.end() && utils::loadPageContent(it->second, body)) {
			contentType = "text/html";
			loaded = true;
		}
	}
	if (!loaded) {
		std::string reason = http::StatusCode::to_reasonPhrase(status);
		std::string message = fallbackBody.empty() ? "요청을 처리할 수 없습니다." : fallbackBody;
		body = std::string("<!DOCTYPE html><html><head><meta charset=\"utf-8\">") + "<title>" +
			   int_tostr(status) + " " + reason + "</title>" +
			   "<style>"
			   "body{font-family:Arial,'Noto Sans KR',sans-serif;background:#f6f7fb;"
			   "color:#333;display:flex;align-items:center;justify-content:center;"
			   "min-height:100vh;margin:0;}"
			   ".box{background:#fff;border-radius:18px;padding:48px "
			   "56px;text-align:center;"
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
	http::Packet response({"HTTP/1.1", status, http::StatusCode::to_reasonPhrase(status)},
						  http::Header(), http::Body());
	if (!contentType.empty()) response.addHeader("Content-Type", contentType);
	if (!body.empty()) response.appendBody(body.c_str(), body.size());
	return response;
}

std::string response::makeCgiResponse(const std::string& cgiResult) {
	cgi::CgiOutput cgiOutput = cgi::OutputValidator::validateCgiOutput(cgiResult);
	std::string httpHeader = cgiOutput.httpHeader;
	std::string body = cgiOutput.body;
	http::StatusCode::Value statusCode = cgiOutput.statusCode;

	if (cgiOutput.error == cgi::CgiOutput::INVALID_FORMAT) {
		throw handler::Exception();
	}

	size_t lineEnd = httpHeader.find("\r\n", 0);
	httpHeader.erase(0, lineEnd + 2);

	std::string statusLine = "HTTP/1.1 " + int_tostr(statusCode) + " " +
							 http::StatusCode::to_reasonPhrase(statusCode) + "\r\n";

	httpHeader +=
		("\r\n"
		 "Content-Length: " +
		 int_tostr(body.size()) +
		 "\r\n"
		 "Server: webserv\r\n");

	return statusLine + httpHeader + "\r\n" + body;
}
