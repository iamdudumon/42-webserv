// response.hpp
#ifndef HANDLER_UTILS_RESPONSE_HPP
#define HANDLER_UTILS_RESPONSE_HPP

#include <string>

#include "../../http/Enums.hpp"
#include "../../http/model/Packet.hpp"

namespace handler {
	namespace utils {
		inline http::Packet makePlainResponse(http::StatusCode::Value status,
											  const std::string& body,
											  const std::string& contentType) {
			http::StatusLine statusLine = {"HTTP/1.1", status,
										   http::StatusCode::to_reasonPhrase(status)};
			http::Packet response(statusLine, http::Header(), http::Body());

			response.addHeader("Content-Type", contentType);
			if (!body.empty()) response.appendBody(body.c_str(), body.size());
			return response;
		}
	}  // namespace utils
}  // namespace handler

#endif
