// PacketLineState.cpp
#include "PacketLineState.hpp"

#include <sstream>

#include "../HttpParser.hpp"

void PacketLineState::parse(HttpParser* parser, const std::string& line) {
	std::istringstream iss(line);
	std::string		   str1, str2, str3;

	iss >> str1 >> str2 >> str3;
	if (HTTP::Method::to_value(str1) !=
		HTTP::Method::UNKNOWN_METHOD) {	 // Request
		HTTP::StartLine startLine = {HTTP::Method::to_value(str1), str2, str3};

		parser->_packet = new HttpPacket(startLine, Header(), Body());
	} else {  // Response
		HTTP::StatusLine statusLine = {"HTTP/1.1",
									   HTTP::StatusCode::to_value(str2), str3};

		parser->_packet = new HttpPacket(statusLine, Header(), Body());
	}
	_done = true;
}

void PacketLineState::handleNextState(HttpParser* parser) {
	if (_done) parser->changeState(new HeaderState());
}
