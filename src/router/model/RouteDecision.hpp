// RouteDecision.hpp
#ifndef ROUTER_MODEL_ROUTEDECISION_HPP
#define ROUTER_MODEL_ROUTEDECISION_HPP

#include <string>
#include <vector>

#include "../../config/model/Config.hpp"
#include "../../http/Enums.hpp"

namespace router {
	struct RouteDecision {
			enum Action {
				ServeFile,
				ServeAutoIndex,
				Cgi,
				Redirect,
				Error
			} action;

			http::StatusCode::Value status;
			const config::Config* server;
			std::string locationPath;
			std::string queryString;

			std::string fsRoot;
			std::string fsPath;
			std::string indexUsed;
			std::string contentTypeHint;

			std::vector<std::string> allowMethods;
			std::string redirectLocation;

			RouteDecision() :
				action(Error), status(http::StatusCode::InternalServerError), server(NULL) {}
	};

}  // namespace router

#endif
