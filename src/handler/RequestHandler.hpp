// RequestHandler.hpp
#ifndef HANDLER_REQUESTHANDLER_HPP
#define HANDLER_REQUESTHANDLER_HPP

#include <map>
#include <vector>

#include "../config/model/Config.hpp"
#include "../http/model/Packet.hpp"
#include "../router/Router.hpp"
#include "builder/AutoIndexBuilder.hpp"
#include "builder/Builder.hpp"
#include "builder/ErrorBuilder.hpp"
#include "builder/FileBuilder.hpp"
#include "builder/RedirectBuilder.hpp"
#include "cgi/Builder.hpp"

namespace router {
	struct RouteDecision;
}

namespace handler {
	class RequestHandler {
		public:
			RequestHandler();
			~RequestHandler();

			router::RouteDecision route(const http::Packet&, const std::vector<config::Config>&, int) const;
			http::Packet handle(const http::Packet&, const std::vector<config::Config>&, int) const;

			void handleCgi(const http::Packet&, const std::vector<config::Config>&, int, int,
						   server::EpollManager&);
			void handleCgiEvent(int, server::EpollManager&);
			int getClientFd(int) const;
			bool isCgiProcess(int) const;
			bool isCgiCompleted(int) const;
			void removeCgiProcess(int);
			std::string getCgiResponse(int);

		private:
			router::Router _router;
			std::map<router::RouteDecision::Action, builder::IBuilder*> _builders;
			builder::IBuilder* _defaultBuilder;
			cgi::Manager _cgiManager;  // CGI 매니저(캡슐화)

			const builder::IBuilder* selectBuilder(router::RouteDecision::Action) const;
	};
}  // namespace handler

#endif
