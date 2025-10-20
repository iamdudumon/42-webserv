// RedirectBuilder.cpp
#include "RedirectBuilder.hpp"

using namespace handler::builder;

http::Packet RedirectBuilder::build(const router::RouteDecision& decision, const http::Packet&,
									const std::vector<config::Config>&) const {
	http::StatusLine statusLine = {"HTTP/1.1", decision.status,
								   http::StatusCode::to_reasonPhrase(decision.status)};
	http::Packet response(statusLine, http::Header(), http::Body());
	std::string msg = std::string("Redirecting to ") + decision.redirectLocation;

	response.addHeader("Location", decision.redirectLocation);
	response.addHeader("Content-Type", "text/plain");
	if (!msg.empty()) response.appendBody(msg.c_str(), msg.size());
	return response;
}
