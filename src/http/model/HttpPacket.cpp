#include "HttpPacket.hpp"

HttpPacket::HttpPacket(const HTTP::StartLine& startLine, const Header& header,
					   const Body& body)
	: _startLine(startLine), _header(header), _body(body), _isRequest(true) {}

HttpPacket::HttpPacket(const HTTP::StatusLine& statusLine, const Header& header,
					   const Body& body)
	: _statusLine(statusLine),
	  _header(header),
	  _body(body),
	  _isRequest(false) {}

const Header& HttpPacket::getHeader() const { return _header; }

const Body& HttpPacket::getBody() const { return _body; }

const HTTP::StartLine& HttpPacket::getStartLine() const { return _startLine; }

const HTTP::StatusLine& HttpPacket::getStatusLine() const {
	return _statusLine;
}

const bool HttpPacket::isRequest() const { return _isRequest; }