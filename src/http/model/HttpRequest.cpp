#include "HttpRequest.hpp"

HttpRequest::HttpRequest(const HTTP::StartLine& startLine, const Header& header,
						 const Body& body)
	: _startLine(startLine), HttpPacket(header, body) {}

const HTTP::StartLine& HttpRequest::getStartLine() const { return _startLine; }
