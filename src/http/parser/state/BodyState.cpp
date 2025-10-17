// BodyState.cpp
#include "BodyState.hpp"

#include "../exception/NeedMoreInput.hpp"
#include "../exception/ParserException.hpp"

namespace http {
	void BodyState::parse(Parser* parser) {
		if (_done) return;
		if (_remain == 0) {
			_done = true;
			return;
		}

		std::string chunk;

		try {
			chunk = parser->readBytes(_remain);
		} catch (const NeedMoreInput&) {
			if (parser->inputEnded())
				throw ParserException("Malformed request: Body incomplete",
									  http::StatusCode::BadRequest);
			throw;
		}
		parser->_packet->appendBody(chunk.data(), chunk.size());
		_remain = 0;
		_done = true;
		parser->_packet->applyBodyLength(parser->_packet->getBody().getData().size());
	}

	void BodyState::handleNextState(Parser* parser) {
		if (_done) parser->changeState(new DoneState());
	}
}  // namespace http
