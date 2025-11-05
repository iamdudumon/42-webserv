#include "Responder.hpp"

using namespace handler::cgi;

Responder::CgiOutput Responder::parseCgiOutput(const std::string& cgiResult) {
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
	std::string statusStr = httpHeader.substr(8, lineEnd - 8);
	http::StatusCode::Value statusCode = static_cast<http::StatusCode::Value>(str_toint(statusStr));
	std::string statusMessage = http::StatusCode::to_reasonPhrase(statusCode);
	if (statusMessage == "Unknown Status") {
		output.error = CgiOutput::INVALID_FORMAT;
		return output;
	}

	output.statusCode = statusCode;
	output.httpHeader = httpHeader;
	output.body = body;
	output.error = CgiOutput::NONE;
	return output;
}

std::string Responder::makeCgiResponse(const std::string& cgiResult) {
	CgiOutput cgiOutput = Responder::parseCgiOutput(cgiResult);
	std::string httpHeader = cgiOutput.httpHeader;
	std::string body = cgiOutput.body;
	http::StatusCode::Value statusCode = cgiOutput.statusCode;

	if (cgiOutput.error == CgiOutput::INVALID_FORMAT) {
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
