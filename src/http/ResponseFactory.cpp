#include "ResponseFactory.hpp"

#include "../utils/file_utils.hpp"
#include "../utils/str_utils.hpp"

using namespace http;

namespace {

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
