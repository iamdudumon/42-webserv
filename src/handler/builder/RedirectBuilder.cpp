#include "RedirectBuilder.hpp"

#include "../../http/response/Factory.hpp"

using namespace handler::builder;

http::Packet RedirectBuilder::build(const router::RouteDecision& decision, const http::Packet&,
									const config::Config&) const {
	return http::response::Factory::createRedirect(decision.status, decision.redirectLocation);
}
