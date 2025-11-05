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
#include "cgi/Executor.hpp"
#include "cgi/ProcessManager.hpp"

namespace router {
	struct RouteDecision;
}

namespace handler {
	class RequestHandler {
		private:
			std::map<router::RouteDecision::Action, builder::IBuilder*> _builders;
			builder::IBuilder* _defaultBuilder;

			const builder::IBuilder* selectBuilder(router::RouteDecision::Action) const;

		public:
			RequestHandler();
			~RequestHandler();

			http::Packet handle(int, const http::Packet&, const router::RouteDecision&,
								const config::Config&) const;
	};
}  // namespace handler

#endif
