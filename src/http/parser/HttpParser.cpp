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
	const char* start = _rawData.c_str();
	const char* end = start + _rawData.length();
	const char* crlf;

	while ((crlf = std::search(start, end, "\r\n", "\r\n" + 2)) != end) {
		std::string line(start, crlf);

		_currentState->parse(this, line);
		_currentState->handleNextState(this);

		if (dynamic_cast<DoneState*>(_currentState)) {
			if (!std::string(crlf + 2, end).empty())
				throw HttpParseException("Invalid Content-Length header value",
										 HTTP::StatusCode::BadRequest);
			return;
		}

		start = crlf + 2;
	}

	if (!dynamic_cast<DoneState*>(_currentState))
		throw HttpParseException("Malformed request: Invalid line ending",
								 HTTP::StatusCode::BadRequest);
}

HttpPacket HttpParser::getResult() { return *_packet; }
