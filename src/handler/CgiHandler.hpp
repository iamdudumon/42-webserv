#ifndef HANDLER_CGIHANDLER_HPP
#define HANDLER_CGIHANDLER_HPP

#include <map>

#include "../config/model/Config.hpp"
#include "../http/model/Packet.hpp"
#include "../router/Router.hpp"
#include "cgi/ProcessManager.hpp"

namespace server {
	class EpollManager;
}

namespace handler {
	class CgiHandler {
		private:
			cgi::ProcessManager _cgiProcessManager;

		public:
			CgiHandler();
			~CgiHandler();

			void handle(int, const http::Packet&, const router::RouteDecision&,
						server::EpollManager&);
			void handleEvent(int, server::EpollManager&);
			int getClientFd(int) const;
			bool isCgiProcess(int) const;
			bool isCgiProcessing(int) const;
			bool isCgiCompleted(int) const;
			void removeCgiProcess(int);
			std::string getCgiResponse(int, const config::Config&);
	};
}  // namespace handler

#endif
