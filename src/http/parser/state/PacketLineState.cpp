// PacketLineState.cpp
#include "PacketLineState.hpp"

#include <sstream>

#include "../exception/NeedMoreInput.hpp"
#include "../exception/ParserException.hpp"
#include "HeaderState.hpp"

namespace http {
	void PacketLineState::parse(Parser* parser) {
		if (_done) return;

		std::string line;
		try {
			line = parser->readLine();
		} catch (const NeedMoreInput&) {
			if (parser->inputEnded())
				throw ParserException("Malformed request: unexpected end of request line",
									  StatusCode::BadRequest);
			throw;
		}
		std::string t1, t2, rest;
		std::istringstream iss(line);
		iss >> t1 >> t2;
		std::getline(iss, rest);
		if (!rest.empty() && rest[0] == ' ') rest.erase(0, 1);

		Method::Value method = Method::to_value(t1);
		if (method != Method::UNKNOWN_METHOD) {
			StartLine startLine = {method, t2, rest};
			parser->_packet = new Packet(startLine, Header(), Body());
		} else if (t1.size() >= 5 && t1.compare(0, 5, "HTTP/") == 0) {
			StatusLine statusLine = {t1, StatusCode::to_value(t2), rest};
			parser->_packet = new Packet(statusLine, Header(), Body());
		} else {
			StartLine startLine = {Method::UNKNOWN_METHOD, t2, rest};
			parser->_packet = new Packet(startLine, Header(), Body());
		}
		_done = true;
	}

	void PacketLineState::handleNextState(Parser* parser) {
		if (_done) parser->changeState(new HeaderState());
	}
}  // namespace http
