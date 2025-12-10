// EventHandler.hpp
#ifndef HANDLER_EVENTHANDLER_HPP
#define HANDLER_EVENTHANDLER_HPP

#include <stdint.h>

#include <map>
#include <string>

#include "../client/Client.hpp"
#include "../config/model/Config.hpp"
#include "../router/Router.hpp"
#include "builder/ResponseBuilder.hpp"
#include "cgi/ProcessManager.hpp"
#include "model/EventResult.hpp"

namespace server {
	class EpollManager;
}

namespace handler {
	class EventHandler {
		private:
			router::Router _router;
			builder::ResponseBuilder _responseBuilder;
			cgi::ProcessManager _cgiProcessManager;
			std::map<int, client::Client*> _clients;

			EventResult handleClientEvent(int, uint32_t, const config::Config*,
										  server::EpollManager&);
			EventResult handleCgiEvent(int, uint32_t, const config::Config*, server::EpollManager&);

			void processRequest(client::Client*, const config::Config*, server::EpollManager&);
			void handleParseError(client::Client*, const config::Config*, http::Parser::Result&);

			std::string readSocket(int, bool&) const;
			client::Client* ensureClient(int, const config::Config*);

		public:
			EventHandler();
			~EventHandler();

			EventResult handleEvent(int, uint32_t, const config::Config*, server::EpollManager&);
			void cleanup(int, server::EpollManager&);
			void checkTimeouts(server::EpollManager&);
	};
}  // namespace handler

#endif
