// BodyState.cpp
#include "BodyState.hpp"

#include <algorithm>

void BodyState::parse(HttpParser* parser, const std::string& line) {
	if (_remain == 0) {
		_done = true;
		return;
	}

	size_t toCopy = std::min(_remain, line.size());
	parser->_packet->appendBody(line.data(), toCopy);
	_remain -= toCopy;
	if (_remain == 0) _done = true;
}

void BodyState::handleNextState(HttpParser* parser) {
	if (_done) parser->changeState(new DoneState());
}
