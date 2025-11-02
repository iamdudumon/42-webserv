// Router.hpp
#ifndef ROUTER_ROUTER_HPP
#define ROUTER_ROUTER_HPP

#include <string>
#include <vector>

#include "../config/model/Config.hpp"
#include "../http/model/Packet.hpp"
#include "model/RouteDecision.hpp"

namespace router {
	class Router {
		private:
			std::string bestLocationPrefix(const config::Config&, const std::string&) const;

			bool ensureRequestIsValid(const http::Packet&, RouteDecision&) const;
			void resolveLocation(const config::Config&, const http::Packet&, std::string&,
								 std::string&, RouteDecision&) const;
			bool validateMethod(const config::Config&, const http::Packet&, const std::string&,
								RouteDecision&) const;
			bool decideResource(const config::Config&, const std::string&, const std::string&,
								RouteDecision&) const;
			std::string parseQueryString(const std::string&) const;
			bool isCgiRequest(const std::string&, const std::string&) const;

		public:
			Router() {}
			RouteDecision route(const http::Packet&, const config::Config&) const;
	};
}  // namespace router

#endif
