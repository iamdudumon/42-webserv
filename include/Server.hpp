#ifndef SERVER_HPP
# define SERVER_HPP

# include <sys/socket.h>
# include <iostream>
# include <netinet/in.h>
# include <string>
# include <unistd.h>

# include "Config.hpp"

class Server {
	private:
		int _serverSocket;
		int _clientSocket;
		int _socketOption;
		int _addressSize;
		struct sockaddr_in _address;
		const int _PORT;
	
		void initAddress();
		int validateFunction(int returnValue);
		void initServer();
		void loopServer();
		std::string readSocket();
		void writeSocket();
	
	public:
		Server(Config config);
		
		void runServer();

};

#endif