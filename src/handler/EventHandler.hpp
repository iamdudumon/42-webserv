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

namespace server {
	class EpollManager;
}

namespace handler {
	class EventHandler {
		public:
			struct Response {
					int fd;
					std::string data;
					bool closeAfterSend;
					explicit Response(int socket = -1, const std::string& raw = std::string(),
									  bool close = false) :
						fd(socket), data(raw), closeAfterSend(close) {}
			};
			struct Result {
					Response response;
					int closeFd;
					Result() : response(), closeFd(-1) {}
			};

		private:
			router::Router _router;
			RequestHandler _requestHandler;
			cgi::ProcessManager _cgiProcessManager;
			std::map<int, http::Parser*> _parsers;
			std::map<int, const config::Config*> _cgiClientConfigs;

			http::Parser* ensureParser(int, const config::Config*);
			std::string readSocket(int) const;
			Result handleClientEvent(int, uint32_t, const config::Config*, server::EpollManager&);
			Result handleCgiEvent(int, uint32_t, const config::Config*, server::EpollManager&);

		public:
			EventHandler();
			~EventHandler();

			Result handleEvent(int, uint32_t, const config::Config*, server::EpollManager&);
			void cleanup(int, server::EpollManager&);
	};
}  // namespace handler

#endif
