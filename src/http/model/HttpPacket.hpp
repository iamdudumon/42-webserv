#ifndef HTTPPACKET_HPP
#define HTTPPACKET_HPP

#include <string>

#include "Body.hpp"
#include "Header.hpp"

class HttpPacket {
	protected:
		std::string _rawData;
		Header		_header;
		Body		_body;

		HttpPacket(const Header&, const Body&);

	public:
		const std::string& getRawData() const;
		const Header&	   getHeader() const;
		const Body&		   getBody() const;
};

#endif