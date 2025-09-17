// Parser.cpp
#include "Parser.hpp"

#include <sstream>

#include "exception/ParserException.hpp"

namespace http {
	Parser::Parser(const std::string& rawData) :
		_currentState(new PacketLineState()), _rawData(rawData), _pos(0), _packet(NULL) {}

	Parser::~Parser() {
		delete _currentState;
		if (_packet) delete _packet;
	}

	void Parser::changeState(ParseState* newState) {
		if (_currentState) {
			delete _currentState;
		}
		_currentState = newState;
	}

	void Parser::parse() {
		while (true) {
			_currentState->parse(this);
			_currentState->handleNextState(this);
			if (dynamic_cast<DoneState*>(_currentState)) break;
		}

		// Done 이후 추가 바이트가 남아있으면(파이프라이닝 미지원) 오류 처리
		if (_pos != _rawData.size())
			throw ParserException("Unexpected extra bytes after HTTP message",
								  http::StatusCode::BadRequest);
	}

	Packet Parser::getResult() {
		return *_packet;
	}

	std::string Parser::readLine() {
		size_t next = _rawData.find("\r\n", _pos);
		if (next == std::string::npos)
			throw ParserException("Malformed request: Invalid line ending",
								  http::StatusCode::BadRequest);
		std::string line = _rawData.substr(_pos, next - _pos);
		_pos = next + 2;

		return line;
	}

	std::string Parser::readBytes(size_t n) {
		if (_pos + n > _rawData.size())
			throw ParserException("Malformed request: Body incomplete",
								  http::StatusCode::BadRequest);
		std::string chunk = _rawData.substr(_pos, n);
		_pos += n;

		return chunk;
	}

	size_t Parser::remaining() const {
		return _rawData.size() - _pos;
	}
}  // namespace http
