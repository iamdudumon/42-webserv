#include "FileBuilder.hpp"

#include "../../http/response/Factory.hpp"
#include "../../utils/file_utils.hpp"

using namespace handler::builder;

http::Packet FileBuilder::build(const router::RouteDecision& decision, const http::Packet&,
								const config::Config& config) const {
	if (decision.fsPath.empty())
		return http::response::Factory::createError(
			http::StatusCode::NotFound, &config,
			http::StatusCode::to_reasonPhrase(http::StatusCode::NotFound),
			http::ContentType::to_string(http::ContentType::CONTENT_TEXT_PLAIN));

	FileInfo file = readFile(decision.fsPath.c_str());
	if (file.error != FileInfo::NONE)
		return http::response::Factory::createError(
			http::StatusCode::NotFound, &config,
			http::StatusCode::to_reasonPhrase(http::StatusCode::NotFound),
			http::ContentType::to_string(http::ContentType::CONTENT_TEXT_PLAIN));
	return http::response::Factory::createFileResponse(decision.status, decision.fsPath,
													   decision.contentTypeHint);
}
