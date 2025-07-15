// HttpPacket.hpp
#ifndef HTTPPACKET_HPP
#define HTTPPACKET_HPP

#include <string>

#include "../types/PacketLine.hpp"
#include "Body.hpp"
#include "Header.hpp"

class HttpPacket {
	private:
		HTTP::StartLine	 _startLine;
		HTTP::StatusLine _statusLine;
		Header			 _header;
		Body			 _body;
		bool			 _isRequest;

		HttpPacket(const HTTP::StartLine&, const Header&, const Body&);
		HttpPacket(const HTTP::StatusLine&, const Header&, const Body&);

	public:
		const HTTP::StartLine&	getStartLine() const;
		const HTTP::StatusLine& getStatusLine() const;
		const Header&			getHeader() const;
		const Body&				getBody() const;
		const bool				isRequest() const;
};

#endif