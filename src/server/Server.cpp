// Server.cpp
#include "Server.hpp"

#include <arpa/inet.h>
#include <sys/epoll.h>

#include <algorithm>
#include <cstring>
#include <iostream>

#include "../http/parser/Parser.hpp"
#include "../http/serializer/Serializer.hpp"
#include "Defaults.hpp"
#include "epoll/exception/EpollException.hpp"
#include "exception/Exception.hpp"
#include "wrapper/SocketWrapper.hpp"

namespace server {
	Server::Server(const std::vector<config::Config>& configs) :
		_configs(configs),
		_clientSocket(-1),
		_socketOption(1),
		_addressSize(sizeof(_serverAddress)) {
		_epollManager.init();
	}

	void Server::initAddress(int index) {
		_serverAddress.sin_family = AF_INET;
		_serverAddress.sin_port = htons(_configs[index].getListen());
		_serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		std::fill(_serverAddress.sin_zero, _serverAddress.sin_zero + 8, 0);
	}

	void Server::initServer() {
		int serverSocket = socket::create(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		socket::setOption(serverSocket, SOL_SOCKET, SO_REUSEADDR, &_socketOption,
						  sizeof(_socketOption));
		socket::bind(serverSocket, reinterpret_cast<sockaddr*>(&_serverAddress),
					 sizeof(_serverAddress));
		socket::listen(serverSocket, 10);
		_serverSockets.insert(serverSocket);
		_epollManager.add(serverSocket);
	}

	void Server::loop() {
		try {
			while (true) {
				_epollManager.wait();
				handleEvents();
			}
		} catch (const Exception& e) {
			std::cerr << e.what() << std::endl;
		} catch (const EpollException& e) {
			std::cerr << e.what() << std::endl;
		} catch (const std::exception& e) {
			std::cerr << e.what() << std::endl;
		} catch (...) {
			std::cerr << "[Error] Unknown Error" << std::endl;
		}
	}

	void Server::handleEvents() {
		for (int i = 0; i < _epollManager.eventCount(); i++) {
			const epoll_event& event = _epollManager.eventAt(i);
			if (_serverSockets.find(event.data.fd) != _serverSockets.end()) {
				_clientSocket =
					socket::accept(event.data.fd, reinterpret_cast<sockaddr*>(&_clientAddress),
								   reinterpret_cast<socklen_t*>(&_addressSize));
				_epollManager.add(_clientSocket);
			} else {
				int clientFd = event.data.fd;
				std::string buffer = readSocket(clientFd);
				if (!buffer.size() || event.events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
					_epollManager.remove(clientFd);
				} else {
					http::Packet httpRequest = convertPacket(buffer);

					int localPort = 0;
					sockaddr_in addr;
					socklen_t len = sizeof(addr);
					if (getsockname(clientFd, reinterpret_cast<sockaddr*>(&addr), &len) == 0) {
						localPort = ntohs(addr.sin_port);
					}

					http::Packet httpResponse =
						_requestHandler.handle(httpRequest, _configs, localPort);
					writePacket(clientFd, httpResponse);
				}
			}
		}
	}

	std::string Server::readSocket(int socketFd) {
		char buffer[defaults::BUFFER_SIZE] = {0};
		int readSize;
		std::string request;

		while ((readSize = ::read(socketFd, buffer, defaults::BUFFER_SIZE)) ==
			   defaults::BUFFER_SIZE) {
			request.append(buffer, readSize);
		}
		if (readSize > 0) request.append(buffer, readSize);
		return request;
	}

	void Server::writePacket(int socketFd, const http::Packet& httpResponse) {
		std::string rawResponse = http::Serializer::serialize(httpResponse);
		writeSocket(socketFd, rawResponse);
	}

	http::Packet Server::convertPacket(const std::string& buffer) {
		http::Parser httpParser(buffer);
		httpParser.parse();
		return httpParser.getResult();
	}

	void Server::writeSocket(int socketFd, const std::string& rawData) {
		::write(socketFd, rawData.c_str(), rawData.size());
	}

	void Server::run() {
		for (size_t i = 0; i < _configs.size(); ++i) {
			initAddress(static_cast<int>(i));
			initServer();
		}
		loop();
	}
}  // namespace server
