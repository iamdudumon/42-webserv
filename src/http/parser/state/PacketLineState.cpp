// PacketLineState.cpp
#include "PacketLineState.hpp"

#include <sstream>

void PacketLineState::parse(HttpParser* parser, const std::string& line) {
	std::istringstream iss(line);
	std::string		   str1, str2, str3;

	iss >> str1 >> str2 >> str3;
	if (HTTP::Method::to_value(str1) !=
		HTTP::Method::UNKNOWN_METHOD)  // Request
		parser->_packet = new HttpPacket(
			{HTTP::Method::to_value(str1), str2, str3}, Header(), Body());
	else  // Response
		parser->_packet =
			new HttpPacket({"HTTP/1.1", HTTP::StatusCode::to_value(str2), str3},
						   Header(), Body());
	_done = true;
}

void PacketLineState::handleNextState(HttpParser* parser) {
	if (_done) parser->changeState(new HeaderState());
}
