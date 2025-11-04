#include "Validator.hpp"

using namespace handler::cgi;

CgiOutput OutputValidator::validateCgiOutput(const std::string& cgiResult) {
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
