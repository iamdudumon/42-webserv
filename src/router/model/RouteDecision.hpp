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
			std::string location_path;

			std::string fs_root;
			std::string fs_path;
			std::string index_used;
			std::string content_type_hint;

			std::vector<std::string> allow_methods;
			std::string redirect_location;

			RouteDecision() :
				action(Error), status(http::StatusCode::InternalServerError), server(NULL) {}
	};

}  // namespace router

#endif
