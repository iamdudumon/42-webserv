// Server.hpp
#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#include "../config/model/Config.hpp"
#include "../handler/MainHandler.hpp"
#include "../http/model/Packet.hpp"
#include "../http/parser/Parser.hpp"
#include "epoll/manager/EpollManager.hpp"

namespace server {
	class Server {
		private:
			std::map<int, config::Config> _configs;
			std::set<int> _serverSockets;
			int _clientSocket;
			int _socketOption;
			int _addressSize;
			sockaddr_in _serverAddress;
			sockaddr_in _clientAddress;
			EpollManager _epollManager;
			handler::MainHandler _mainHandler;
			std::map<int, http::Parser*> _parsers;
			const config::Config* findConfig(int) const;

			void initServer(int);
			void loop();
			void handleEvents();
			std::string readSocket(int);
			void sendResponse(int, const http::Packet&);
			void sendResponse(int, const std::string&);
			void cleanupClient(int);
			http::Parser* ensureParser(int);

		public:
			explicit Server(const std::map<int, config::Config>&);

			void run();
	};
}  // namespace server

#endif
