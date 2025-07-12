//Server.hpp
#pragma once
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

#include "../../include/Config.hpp"
#include "manager/EpollManager.hpp"
#include "../../include/ConfigParser.hpp"
#include "../../include/SystemConfig.hpp"
#include "exception/ServerException.hpp"
#include "../http/model/HttpRequest.hpp"
#include "../http/model/HttpResponse.hpp"

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
		void		writeHttpPacket(int, HttpResponse);
		void		handleEvents();
		void		writeSocket(int, std::string);
		int			validateFunction(int, std::string);
		std::string readSocket(int);
		HttpRequest readHttpPacket(int);

	public:
		Server(Config config);

		void runServer();
};

#endif