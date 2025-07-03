#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>

#include "../types/PacketLine.hpp"
#include "HttpPacket.hpp"

class HttpRequest : public HttpPacket {
	private:
		HTTP::StartLine _startLine;

	public:
		const HTTP::StartLine& getStartLine() const;
};

#endif