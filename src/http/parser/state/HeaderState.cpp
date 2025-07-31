// HaederState.cpp
#include "HeaderState.hpp"

void HeaderState::parse(HttpParser* parser, const std::string& line) {
	if (line.empty()) {
		_done = true;
		return;
	}
	size_t sep = line.find(':');
	if (sep == std::string::npos) return;

	std::string key = line.substr(0, sep);
	std::string value = line.substr(sep + 1);

	while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
		value.erase(0, 1);
	while (!value.empty() &&
		   (value[value.size() - 1] == ' ' || value[value.size() - 1] == '\t'))
		value.erase(value.size() - 1);

	parser->_packet->addHeader(key, value);
}

void HeaderState::handleNextState(HttpParser* parser) {
	std::string lentghStr = parser->_packet->getHeader().get("Content-Length");
	size_t		contentLength = lentghStr != "" ? str_toint(lentghStr) : 0;

	if (_done) parser->changeState(new BodyState(contentLength));
}
