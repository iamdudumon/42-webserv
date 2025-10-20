// Parser.cpp
#include "Parser.hpp"

#include <sstream>

#include "exception/NeedMoreInput.hpp"
#include "exception/ParserException.hpp"

namespace http {
	Parser::Parser() :
		_currentState(new PacketLineState()),
		_rawData(),
		_pos(0),
		_packet(NULL),
		_complete(false),
		_inputEnded(false) {}

	Parser::~Parser() {
		delete _currentState;
		if (_packet) delete _packet;
	}

	Parser::Result Parser::parse() {
		Result outcome;

		try {
			if (!_complete) {
				while (true) {
					_currentState->parse(this);
					_currentState->handleNextState(this);
					if (dynamic_cast<DoneState*>(_currentState)) {
						_complete = true;
						break;
					}
				}
			}
		} catch (const NeedMoreInput&) {
			if (_inputEnded) {
				outcome.status = Result::Error;
				outcome.errorCode = http::StatusCode::BadRequest;
				outcome.errorMessage = "Malformed request: message incomplete";
			} else
				outcome.status = Result::Incomplete;
			return outcome;
		} catch (const ParserException& e) {
			outcome.status = Result::Error;
			outcome.errorCode = e.getStatusCode();
			outcome.errorMessage = e.what();
			return outcome;
		}

		if (_complete) {
			outcome.status = Result::Completed;
			if (_packet) outcome.packet = *_packet;
			return outcome;
		}
		outcome.status = Result::Incomplete;
		return outcome;
	}

	void Parser::changeState(ParseState* newState) {
		if (_currentState) delete _currentState;
		_currentState = newState;
	}

	void Parser::append(const std::string& chunk) {
		_rawData.append(chunk);
		if (!chunk.empty()) _inputEnded = false;
	}

	void Parser::markEndOfInput() {
		_inputEnded = true;
	}

	bool Parser::inputEnded() const {
		return _inputEnded;
	}

	std::string Parser::tail() const {
		if (_pos >= _rawData.size()) return std::string();
		return _rawData.substr(_pos);
	}

	std::string Parser::readLine() {
		size_t next = _rawData.find("\r\n", _pos);
		if (next == std::string::npos) throw NeedMoreInput();
		std::string line = _rawData.substr(_pos, next - _pos);
		_pos = next + 2;
		return line;
	}

	std::string Parser::readBytes(size_t n) {
		if (_pos + n > _rawData.size()) throw NeedMoreInput();
		std::string chunk = _rawData.substr(_pos, n);
		_pos += n;
		return chunk;
	}
}  // namespace http
