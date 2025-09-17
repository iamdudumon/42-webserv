// BodyState.cpp
#include "BodyState.hpp"

#include <algorithm>

namespace http {
	void BodyState::parse(Parser* parser) {
		if (_remain == 0) {
			_done = true;
			return;
		}

		std::string chunk = parser->readBytes(_remain);
		parser->_packet->appendBody(chunk.data(), chunk.size());
		_remain = 0;
		_done = true;
	}

	void BodyState::handleNextState(Parser* parser) {
		if (_done) parser->changeState(new DoneState());
	}
}  // namespace http
