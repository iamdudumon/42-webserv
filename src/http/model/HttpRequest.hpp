#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>

#include "../types/PacketLine.hpp"
#include "HttpPacket.hpp"

class HttpRequest : public HttpPacket {
	private:
		HTTP::StartLine _startLine;

	public:
		HttpRequest(const HTTP::StartLine&, const Header&, const Body&);

		const HTTP::StartLine& getStartLine() const;
};

#endif
