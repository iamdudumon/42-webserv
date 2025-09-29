// Server.hpp
#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <set>
#include <string>
#include <vector>

#include "../config/model/Config.hpp"
#include "../handler/RequestHandler.hpp"
#include "../http/model/Packet.hpp"
#include "epoll/manager/EpollManager.hpp"

namespace server {
	class Server {
		private:
			std::vector<config::Config> _configs;
			std::set<int> _serverSockets;
			int _clientSocket;
			int _socketOption;
			int _addressSize;
			sockaddr_in _serverAddress;
			sockaddr_in _clientAddress;
			EpollManager _epollManager;
			handler::RequestHandler _requestHandler;

			void initAddress(int);
			void initServer();
			void loop();
			void writePacket(int, const http::Packet&);
			void handleEvents();
			void writeSocket(int, const std::string&);
			std::string readSocket(int);
			http::Packet convertPacket(std::string&);

		public:
			explicit Server(const std::vector<config::Config>&);

			void run();
	};
}  // namespace server

#endif
