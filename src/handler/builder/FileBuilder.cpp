// FileBuilder.cpp
#include "FileBuilder.hpp"

#include "../utils/response.hpp"

using namespace handler::builder;

http::Packet FileBuilder::build(const router::RouteDecision& decision, const http::Packet&,
								const config::Config& config) const {
	std::string fileData;
	if (!utils::loadPageContent(decision.fsPath, fileData)) {
		return utils::makeErrorResponse(
			http::StatusCode::NotFound, &config,
			http::StatusCode::to_reasonPhrase(http::StatusCode::NotFound),
			http::ContentType::to_string(http::ContentType::CONTENT_TEXT_PLAIN));
	}

	http::StatusLine statusLine = {"HTTP/1.1", decision.status,
								   http::StatusCode::to_reasonPhrase(decision.status)};
	http::Packet response(statusLine, http::Header(), http::Body());

	response.addHeader("Content-Type", decision.contentTypeHint.empty() ? "application/octet-stream"
																		: decision.contentTypeHint);
	if (!fileData.empty()) response.appendBody(fileData.data(), fileData.size());
	return response;
}
