// Serializer.cpp
#include "Serializer.hpp"

#include <sstream>
#include <stdexcept>

#include "../../utils/str_utils.hpp"

namespace http {
	std::string Serializer::serialize(const Packet& packet, bool keepAlive) {
		if (packet.isRequest()) throw std::logic_error("Serializer: request packet unsupported");

		std::stringstream ss;
		const std::map<std::string, std::string>& headers = packet.getHeader().getHeaders();
		const std::vector<unsigned char>& bodyData = packet.getBody().getData();
		const size_t bodyLen = bodyData.size();
		const StatusLine statusLine = packet.getStatusLine();
		bool hasServer = false;
		bool hasContentLength = false;
		bool hasConnection = false;

		ss << statusLine.version << " " << StatusCode::to_string(statusLine.statusCode) << " "
		   << statusLine.reasonPhrase << "\r\n";
		for (std::map<std::string, std::string>::const_iterator it = headers.begin();
			 it != headers.end(); ++it) {
			std::string keyLower = to_lower(it->first);
			if (keyLower == "connection") {
				hasConnection = true;
				continue;
			}
			if (keyLower == "content-length") {
				hasContentLength = true;
				continue;
			}
			if (keyLower == "server") hasServer = true;
			ss << it->first << ": " << it->second << "\r\n";
		}
		if (!hasServer) ss << "Server: webserv" << "\r\n";
		if (bodyLen > 0 || hasContentLength) ss << "Content-Length: " << bodyLen << "\r\n";
		if (!hasConnection) ss << "Connection: " << (keepAlive ? "keep-alive" : "close") << "\r\n";
		ss << "\r\n";
		if (!bodyData.empty())
			ss.write(reinterpret_cast<const char*>(&bodyData[0]), bodyData.size());

		return ss.str();
	}
}  // namespace http
