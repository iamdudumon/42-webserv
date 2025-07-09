#include "HttpPacket.hpp"

HttpPacket::HttpPacket(const Header& header, const Body& body)
	: _header(header), _body(body) {}

const std::string& HttpPacket::getRawData() const { return _rawData; }

const Header& HttpPacket::getHeader() const { return _header; }

const Body& HttpPacket::getBody() const { return _body; }
