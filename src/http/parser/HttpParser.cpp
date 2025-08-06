// HttpParser.cpp
#include "HttpParser.hpp"

#include <sstream>

#include "exception/HttpParseException.hpp"

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
	size_t offset = 0;
	size_t next;

	while ((next = _rawData.find("\r\n", offset)) != std::string::npos) {
		std::string line = _rawData.substr(offset, next - offset);
		_currentState->parse(this, line);
		_currentState->handleNextState(this);

		if (dynamic_cast<DoneState*>(_currentState)) {
			if (next + 2 < _rawData.size())
				throw HttpParseException("Invalid Content-Length header value",
										 HTTP::StatusCode::BadRequest);
			return;
		}
		offset = next + 2;
	}

	if (!dynamic_cast<DoneState*>(_currentState))
		throw HttpParseException("Malformed request: Invalid line ending",
								 HTTP::StatusCode::BadRequest);
}

HttpPacket HttpParser::getResult() { return *_packet; }
