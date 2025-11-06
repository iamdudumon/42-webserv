// EventHandler.hpp
#ifndef HANDLER_EVENTHANDLER_HPP
#define HANDLER_EVENTHANDLER_HPP

#include <stdint.h>

#include <map>
#include <string>

#include "../config/model/Config.hpp"
#include "../http/parser/Parser.hpp"
#include "../router/Router.hpp"
#include "RequestHandler.hpp"
#include "cgi/ProcessManager.hpp"
#include "model/EventResult.hpp"

namespace server {
	class EpollManager;
}

namespace handler {
	class EventHandler {
		private:
			router::Router _router;
			RequestHandler _requestHandler;
			cgi::ProcessManager _cgiProcessManager;
			std::map<int, http::Parser*> _parsers;
			struct CgiContext {
					const config::Config* config;
					bool keepAlive;
					CgiContext() : config(NULL), keepAlive(false) {}
					CgiContext(const config::Config* cfg, bool ka) : config(cfg), keepAlive(ka) {}
			};
			std::map<int, CgiContext> _cgiContexts;

			http::Parser* ensureParser(int, const config::Config*);
			std::string readSocket(int) const;
			EventResult handleClientEvent(int, uint32_t, const config::Config*,
										  server::EpollManager&);
			EventResult handleCgiEvent(int, uint32_t, const config::Config*, server::EpollManager&);

		public:
			EventHandler();
			~EventHandler();

			EventResult handleEvent(int, uint32_t, const config::Config*, server::EpollManager&);
			void cleanup(int, server::EpollManager&);
	};
}  // namespace handler

#endif
