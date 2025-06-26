#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <string>

#include "Config.hpp"
#include "HttpParser.hpp"

class Server {
	private:
		const int		   _PORT;
		int				   _serverSocket;
		int				   _clientSocket;
		int				   _socketOption;
		int				   _addressSize;
		struct sockaddr_in _address;

		void		initAddress();
		void		initServer();
		void		loopServer();
		void		writeHttpPacket();
		void		writeSocket(std::string rawData);
		int			validateFunction(int returnValue);
		std::string readSocket();
		HttpRequest readHttpPacket();

	public:
		Server(Config config);

		void runServer();
};

#endif