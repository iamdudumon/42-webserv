// MainHandler.hpp
#ifndef HANDLER_MAINHANDLER_HPP
#define HANDLER_MAINHANDLER_HPP

#include <map>

#include "../config/model/Config.hpp"
#include "../http/Enums.hpp"
#include "../http/model/Packet.hpp"
#include "../router/Router.hpp"
#include "CgiHandler.hpp"
#include "RequestHandler.hpp"

namespace server {
	class EpollManager;
}

namespace handler {
	class MainHandler {
		private:
			router::Router _router;
			RequestHandler _requestHandler;
			CgiHandler _cgiHandler;

		public:
			MainHandler() {}

			bool handle(int, const http::Packet&, const config::Config&, server::EpollManager&,
						http::Packet&);
			void handleCgiEvent(int, server::EpollManager&);
			int getClientFd(int) const;
			bool isCgiProcess(int) const;
			bool isCgiProcessing(int) const;
			bool isCgiCompleted(int) const;
			void removeCgiProcess(int);
			std::string getCgiResponse(int, const config::Config&);
	};
}  // namespace handler

#endif
