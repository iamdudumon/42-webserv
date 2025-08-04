// HttpParser.cpp
#include "HttpParser.hpp"

#include <sstream>

HttpParser::HttpParser(const std::string& rawData)
	: _currentState(new PacketLineState()), _rawData(rawData), _packet(NULL) {}

HttpParser::~HttpParser() {
	delete _currentState;
	if (_packet) delete _packet;
}

void HttpParser::changeState(ParseState* newState) {
	if (_currentState) {
		delete _currentState;
	}
	_currentState = newState;
}

void HttpParser::parse() {
	const char* start = _rawData.c_str();
	const char* end = start + _rawData.length();
	const char* crlf;

	while ((crlf = std::search(start, end, "\r\n", "\r\n" + 2)) != end) {
		std::string line(start, crlf);

		_currentState->parse(this, line);
		_currentState->handleNextState(this);

		if (dynamic_cast<DoneState*>(_currentState)) break;

		start = crlf + 2;
	}

	_currentState->parse(this, std::string());
}

HttpPacket HttpParser::getResult() { return *_packet; }
