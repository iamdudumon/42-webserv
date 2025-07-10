#include "HttpResponse.hpp"

HttpResponse::HttpResponse(const HTTP::StatusLine& statusLine,
						   const Header& header, const Body& body)
	: _statusLine(statusLine), HttpPacket(header, body) {}

const HTTP::StatusLine& HttpResponse::getStatusLine() const {
	return _statusLine;
}
