// CgiBuilder.cpp
#include "CgiBuilder.hpp"

#include "../utils/response.hpp"

using namespace handler::builder;

http::Packet CgiBuilder::build(const router::RouteDecision& decision, const http::Packet&,
							   const std::vector<config::Config>&) const {
	return utils::makePlainResponse(decision.status, "CGI not implemented",
									http::ContentType::to_string(
										http::ContentType::CONTENT_TEXT_PLAIN));
}
