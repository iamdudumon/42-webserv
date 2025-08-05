//Server.hpp
#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "../config/model/Config.hpp"
#include "epoll/manager/EpollManager.hpp"
#include "../config/ConfigParser.hpp"
#include "../../include/SystemConfig.hpp"
#include "exception/ServerException.hpp"
#include "../http/model/HttpPacket.hpp"
#include "../http/parser/HttpParser.hpp"
#include "wrapper/SocketWrapper.hpp"

class Server {
	private:
		const int		   _PORT;
		int				   _serverSocket;
		int				   _clientSocket;
		int				   _socketOption;
		int				   _addressSize;
		struct sockaddr_in _serverAddress;
		struct sockaddr_in _clientAddress;
		EpollManager	   _epollManager;

		void		initAddress();
		void		initServer();
		void		loopServer();
		void		writeHttpPacket(int, HttpPacket);
		void		handleEvents();
		void		writeSocket(int, std::string);
		std::string readSocket(int);
		HttpPacket readHttpPacket(int);

	public:
		Server(Config config);

		void runServer();
};

#endif