// Packet.hpp
#ifndef HTTP_MODEL_PACKET_HPP
#define HTTP_MODEL_PACKET_HPP

#include <string>

#include "Body.hpp"
#include "Header.hpp"
#include "PacketLine.hpp"

namespace http {
	class Packet {
		private:
			StartLine _startLine;
			StatusLine _statusLine;
			Header _header;
			Body _body;
			bool _isRequest;

		public:
			Packet(const StartLine&, const Header&, const Body&);
			Packet(const StatusLine&, const Header&, const Body&);

			const StartLine& getStartLine() const;
			const StatusLine& getStatusLine() const;
			const Header& getHeader() const;
			const Body& getBody() const;
			bool isRequest() const;

			void addHeader(const std::string&, const std::string&);
			void appendBody(const char*, size_t);
			void applyBodyLength(size_t);
			void applyBodyType(http::ContentType::Value);
	};
}  // namespace http

#endif
