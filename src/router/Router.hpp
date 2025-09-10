// Router.hpp
#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <vector>

#include "../http/model/HttpPacket.hpp"
#include "./model/RouteDecision.hpp"

class Router {
	public:
		Router() {}
		RouteDecision route(const HttpPacket&, const std::vector<Config>&, int) const;

	private:
		const Config* selectServer(const std::vector<Config>&, int) const;
		std::string bestLocationPrefix(const Config&, const std::string&) const;
};

#endif
