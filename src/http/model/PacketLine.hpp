// PacketLine.hpp
#ifndef HTTP_MODEL_PACKETLINE_HPP
#define HTTP_MODEL_PACKETLINE_HPP

#include <string>

#include "../Enums.hpp"

namespace http {
	struct StartLine {
			http::Method::Value method;
			std::string target;
			std::string version;
	};

	struct StatusLine {
			std::string version;
			http::StatusCode::Value statusCode;
			std::string reasonPhrase;
	};
}

#endif
