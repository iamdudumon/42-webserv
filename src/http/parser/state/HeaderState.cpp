// HaederState.cpp
#include "HeaderState.hpp"

#include "../../../utils/str_utils.hpp"
#include "../exception/HttpParseException.hpp"

void HeaderState::parse(HttpParser* parser, const std::string& line) {
	if (line.empty()) {
		_done = true;
		return;
	}

	size_t sep = line.find(':');
	if (sep == std::string::npos || sep == 0) return;

	std::string key = line.substr(0, sep);
	std::string value = line.substr(sep + 1);

	value.erase(0, value.find_first_not_of(" 	"));
	value.erase(value.find_last_not_of(" 	") + 1);

	parser->_packet->addHeader(key, value);
}

void HeaderState::handleNextState(HttpParser* parser) {
	if (!_done) return;
	if (parser->_packet->getHeader().get("host").empty())
		throw HttpParseException("Host header is missing",
								 HTTP::StatusCode::BadRequest);

	std::string lentghStr = parser->_packet->getHeader().get("Content-Length");
	size_t		contentLength = lentghStr != "" ? str_toint(lentghStr) : 0;
	HTTP::ContentType::Value contentType = HTTP::ContentType::to_value(
		parser->_packet->getHeader().get("Content-Type"));

	parser->_packet->applyBodyLength(contentLength);
	parser->_packet->applyBodyType(contentType);
	if (contentLength == 0)
		parser->changeState(new DoneState());
	else
		parser->changeState(new BodyState(contentLength));
}
