// Packet.cpp
#include "Packet.hpp"

#include <stdexcept>

namespace http {
	Packet::Packet(const StartLine& startLine, const Header& header, const Body& body) :
		_startLine(startLine), _header(header), _body(body), _isRequest(true) {}

	Packet::Packet(const StatusLine& statusLine, const Header& header, const Body& body) :
		_statusLine(statusLine), _header(header), _body(body), _isRequest(false) {}

	const Header& Packet::getHeader() const {
		return _header;
	}

	const Body& Packet::getBody() const {
		return _body;
	}

	const StartLine& Packet::getStartLine() const {
		if (!_isRequest) throw std::logic_error("Packet: not a request.");
		return _startLine;
	}

	const StatusLine& Packet::getStatusLine() const {
		if (_isRequest) throw std::logic_error("Packet: not a response.");
		return _statusLine;
	}

	bool Packet::isRequest() const {
		return _isRequest;
	}

	void Packet::addHeader(const std::string& key, const std::string& value) {
		_header.set(key, value);
	}

	void Packet::appendBody(const char* data, size_t len) {
		_body.append(data, len);
	}

	void Packet::applyBodyLength(size_t len) {
		_body.setLength(len);
	}

	void Packet::applyBodyType(http::ContentType::Value type) {
		_body.setType(type);
	}
}  // namespace http
