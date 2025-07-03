#pragma once
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>

#include "../types/PacketLine.hpp"
#include "HttpPacket.hpp"

class HttpResponse : public HttpPacket {
	private:
		HTTP::StatusLine _statusLine;

	public:
		const HTTP::StatusLine& getStatusLine() const;
};

#endif