// HttpSerializer.cpp
#include "HttpSerializer.hpp"

#include <sstream>

#include "../../utils/str_utils.hpp"

std::string HttpSerializer::serialize(const HttpPacket& packet) {
	std::stringstream						  ss;
	const std::map<std::string, std::string>& headers =
		packet.getHeader().getHeaders();
	const std::vector<unsigned char>& bodyData = packet.getBody().getData();

	if (!packet.isRequest()) {
		const HTTP::StatusLine& statusLine = packet.getStatusLine();
		ss << statusLine.version << " "
		   << HTTP::StatusCode::to_string(statusLine.statusCode) << " "
		   << statusLine.reasonPhrase << "\r\n";
	} else
		return NULL;

	for (std::map<std::string, std::string>::const_iterator it =
			 headers.begin();
		 it != headers.end(); ++it) {
		ss << it->first << ": " << it->second << "\r\n";
	}

	ss << "\r\n";

	if (!bodyData.empty()) {
		ss.write(reinterpret_cast<const char*>(&bodyData[0]), bodyData.size());
	}

	return ss.str();
}
