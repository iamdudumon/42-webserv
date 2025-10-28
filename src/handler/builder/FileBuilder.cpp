// FileBuilder.cpp
#include "FileBuilder.hpp"

#include <fstream>
#include <sstream>

#include "../utils/response.hpp"

using namespace handler::builder;

http::Packet FileBuilder::build(const router::RouteDecision& decision, const http::Packet&,
								const std::map<int, config::Config>&) const {
	std::ifstream ifs(decision.fsPath.c_str(), std::ios::in | std::ios::binary);
	if (!ifs.is_open()) {
		return utils::makePlainResponse(
			http::StatusCode::NotFound,
			http::StatusCode::to_reasonPhrase(http::StatusCode::NotFound),
			http::ContentType::to_string(http::ContentType::CONTENT_TEXT_PLAIN));
	}

	std::ostringstream ss;
	ss << ifs.rdbuf();
	std::string fileData = ss.str();
	http::StatusLine statusLine = {"HTTP/1.1", decision.status,
								   http::StatusCode::to_reasonPhrase(decision.status)};
	http::Packet response(statusLine, http::Header(), http::Body());

	response.addHeader("Content-Type", decision.contentTypeHint.empty() ? "application/octet-stream"
																		: decision.contentTypeHint);
	if (!fileData.empty()) response.appendBody(fileData.data(), fileData.size());
	return response;
}
