#include "Server.hpp"

Server::Server(Config config)
	: _PORT(config.getListen()),
	  _socketOption(1),
	  _addressSize(sizeof(_serverAddress)) {
	initAddress();
}

void Server::initAddress() {
	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_port = htons(_PORT);
	_serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	std::fill(_serverAddress.sin_zero, _serverAddress.sin_zero + 8, 0);
}

void Server::initServer() {
	_serverSocket = SocketWrapper::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	SocketWrapper::setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &_socketOption, sizeof(_socketOption));
	SocketWrapper::bind(_serverSocket, (struct sockaddr*) &_serverAddress, sizeof(_serverAddress));
	SocketWrapper::listen(_serverSocket, 10);
	_epollManager.initEpoll(_serverSocket);
}

void Server::loopServer() {
	try {
		while (1) {
			_epollManager.waitEvent();
			handleEvents();
		}
	} catch (const ServerException& e) {
		std::cout << e.what() << std::endl;
	} catch (const EpollException& e) {
		std::cout << e.what() << std::endl;
	} catch (...) {
		std::cout << "[Error] Unknown Error" << std::endl;
	}
}

void Server::handleEvents() {
	for (int i = 0; i < _epollManager.getEventCount(); i++) {
		if (_epollManager.getEpollEventsAt(i).data.fd == _serverSocket) {
			_clientSocket = SocketWrapper::accept(_serverSocket, (struct sockaddr*) &_clientAddress, (socklen_t*) &_addressSize);
			std::cout << "new client :" << _clientSocket << std::endl;
			_epollManager.addEpollFd(_clientSocket);
		} else {
			HttpPacket httpRequest =
				readHttpPacket(_epollManager.getEpollEventsAt(i).data.fd);
			if (_epollManager.getEpollEventsAt(i).events &
				(EPOLLRDHUP | EPOLLHUP | EPOLLERR))
				_epollManager.deleteEpollFd(
					_epollManager.getEpollEventsAt(i).data.fd);
			else {
				HttpPacket httpResponse("test", "test", 200);
				writeHttpPacket(_epollManager.getEpollEventsAt(i).data.fd,
								httpResponse);
			}
		}
	}
}

std::string Server::readSocket(int socketFd) {
	char		buffer[SystemConfig::Size::BUFFER_SIZE] = {0};
	int			readSize;
	std::string request = "";

	while (
		(readSize = read(socketFd, buffer, SystemConfig::Size::BUFFER_SIZE)) ==
		SystemConfig::Size::BUFFER_SIZE) {
		request.append(buffer);
	}
	request.append(buffer);
	return request;
}

void Server::writeHttpPacket(int socketFd, HttpPacket httpResponse) {
	writeSocket(socketFd, httpResponse.getRawData());
}

HttpPacket Server::readHttpPacket(int socketFd) {
	HttpParser httpParser(readSocket(socketFd));
	httpParser.parse();
	HttpPacket httpRequest = httpParser.getResult();
	return httpRequest;
}

void Server::writeSocket(int socketFd, std::string rawData) {
	write(socketFd, rawData.c_str(), rawData.size());
}

void Server::runServer() {
	initServer();
	loopServer();
}