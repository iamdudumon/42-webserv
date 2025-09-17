// PacketLineState.cpp
#include "PacketLineState.hpp"

#include <sstream>

namespace http {
	void PacketLineState::parse(Parser* parser) {
		std::string line = parser->readLine();
		std::string t1, t2, rest;
		std::istringstream iss(line);
		iss >> t1 >> t2;
		std::getline(iss, rest);
		if (!rest.empty() && rest[0] == ' ') rest.erase(0, 1);

		if (http::Method::to_value(t1) != http::Method::UNKNOWN_METHOD) {
			http::StartLine startLine = {http::Method::to_value(t1), t2, rest};
			parser->_packet = new Packet(startLine, Header(), Body());
		} else {
			http::StatusLine statusLine = {t1, http::StatusCode::to_value(t2), rest};
			parser->_packet = new Packet(statusLine, Header(), Body());
		}
		_done = true;
	}

	void PacketLineState::handleNextState(Parser* parser) {
		if (_done) parser->changeState(new HeaderState());
	}
}  // namespace http
