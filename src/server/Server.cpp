// Server.cpp
#include "Server.hpp"

#include <arpa/inet.h>
#include <signal.h>
#include <sys/epoll.h>

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <iostream>

#include "../handler/utils/response.hpp"
#include "../http/Enums.hpp"
#include "../http/parser/Parser.hpp"
#include "../http/parser/exception/NeedMoreInput.hpp"
#include "../http/parser/exception/ParserException.hpp"
#include "../http/serializer/Serializer.hpp"
#include "Defaults.hpp"
#include "epoll/exception/EpollException.hpp"
#include "exception/Exception.hpp"
#include "wrapper/SocketWrapper.hpp"

using namespace server;

Server::Server(const std::map<int, config::Config>& configs) :
	_configs(configs), _clientSocket(-1), _socketOption(1), _addressSize(sizeof(_serverAddress)) {}

void Server::initServer(int port) {
	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_port = htons(port);
	_serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	std::fill(_serverAddress.sin_zero, _serverAddress.sin_zero + 8, 0);

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
	} catch (const server::Exception& e) {
		std::cerr << e.what() << std::endl;
	} catch (const server::EpollException& e) {
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
		int fd = event.data.fd;

		if (_mainHandler.isCgiProcess(fd)) {
			int clientFd = _mainHandler.getClientFd(fd);
			_mainHandler.handleCgiEvent(fd, _epollManager);
			if (clientFd != -1 && _mainHandler.isCgiCompleted(clientFd)) {
				std::string cgiResponse = _mainHandler.getCgiResponse(clientFd, _configs);
				sendResponse(clientFd, cgiResponse);
				_mainHandler.removeCgiProcess(clientFd);
				_epollManager.remove(clientFd);
			}
			continue;
		}

		if (_serverSockets.find(event.data.fd) != _serverSockets.end()) {
			_clientSocket =
				socket::accept(event.data.fd, reinterpret_cast<sockaddr*>(&_clientAddress),
							   reinterpret_cast<socklen_t*>(&_addressSize));
			_epollManager.add(_clientSocket);
			continue;
		}

		int clientFd = event.data.fd;
		if (_mainHandler.isCgiProcessing(clientFd)) continue;

		std::string buffer = readSocket(clientFd);
		bool disconnected = (event.events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) != 0;
		http::Parser* parser = ensureParser(clientFd);

		if (!buffer.empty()) parser->append(buffer);
		if (disconnected) parser->markEndOfInput();
		if (buffer.empty() && !disconnected) continue;

		bool alreadyCleaned = false;
		while (true) {
			http::Parser::Result parseResult = parser->parse();

			switch (parseResult.status) {
				case http::Parser::Result::Incomplete:
					break;
				case http::Parser::Result::Error: {
					const std::string& body =
						parseResult.errorMessage.empty()
							? http::StatusCode::to_reasonPhrase(parseResult.errorCode)
							: parseResult.errorMessage;
					http::Packet errorPacket = handler::utils::makePlainResponse(
						parseResult.errorCode, body,
						http::ContentType::to_string(http::ContentType::CONTENT_TEXT_PLAIN));

					sendResponse(clientFd, errorPacket);
					cleanupClient(clientFd);
					parser = NULL;
					alreadyCleaned = true;
					break;
				}
				case http::Parser::Result::Completed: {
					http::Packet httpRequest = parseResult.packet;
					std::string remainder = parseResult.leftover;
					bool ended = parseResult.endOfInput;

					int localPort = 0;
					sockaddr_in addr;
					socklen_t len = sizeof(addr);
					if (getsockname(clientFd, reinterpret_cast<sockaddr*>(&addr), &len) == 0)
						localPort = ntohs(addr.sin_port);

					const config::Config* config = findConfig(localPort);
					if (!config) {
						http::Packet errorPacket = handler::utils::makePlainResponse(
							http::StatusCode::InternalServerError,
							http::StatusCode::to_reasonPhrase(
								http::StatusCode::InternalServerError),
							http::ContentType::to_string(http::ContentType::CONTENT_TEXT_PLAIN));
						sendResponse(clientFd, errorPacket);
						cleanupClient(clientFd);
						parser = NULL;
						alreadyCleaned = true;
						break;
					}

					http::Packet httpResponse((http::StatusLine()), http::Header(), http::Body());
					bool hasResponse = _mainHandler.handle(clientFd, httpRequest, *config,
														   _epollManager, httpResponse);
					if (hasResponse) {
						sendResponse(clientFd, httpResponse);
					} else {
						if (ended) cleanupClient(clientFd);
						alreadyCleaned = true;
						break;
					}

					if (!remainder.empty()) {
						parser->append(remainder);
						if (ended) parser->markEndOfInput();
						continue;
					}
					if (ended) {
						cleanupClient(clientFd);
						alreadyCleaned = true;
					}
					break;
				}
			}
			break;
		}

		if (alreadyCleaned) continue;
		if (disconnected) cleanupClient(clientFd);
	}
}

const config::Config* Server::findConfig(int localPort) const {
	std::map<int, config::Config>::const_iterator it = _configs.find(localPort);
	if (it != _configs.end()) return &it->second;
	if (!_configs.empty()) return &(_configs.begin()->second);
	return NULL;
}

std::string Server::readSocket(int socketFd) {
	char buffer[defaults::BUFFER_SIZE] = {0};
	std::string request;

	while (true) {
		ssize_t readSize = ::read(socketFd, buffer, defaults::BUFFER_SIZE);

		if (readSize > 0) {
			request.append(buffer, readSize);
			if (readSize < defaults::BUFFER_SIZE) break;
		} else if (readSize == 0)
			break;
		else {
			if (errno == EAGAIN || errno == EWOULDBLOCK) break;
			return std::string();
		}
	}
	return request;
}

void Server::sendResponse(int socketFd, const http::Packet& httpResponse) {
	std::string rawResponse = http::Serializer::serialize(httpResponse);
	::write(socketFd, rawResponse.c_str(), rawResponse.size());
}

void Server::sendResponse(int socketFd, const std::string& rawResponse) {
	::write(socketFd, rawResponse.c_str(), rawResponse.size());
}

void Server::cleanupClient(int fd) {
	std::map<int, http::Parser*>::iterator it = _parsers.find(fd);

	if (it != _parsers.end()) {
		delete it->second;
		_parsers.erase(it);
	}
	_mainHandler.removeCgiProcess(fd);
	_epollManager.remove(fd);
}

http::Parser* Server::ensureParser(int fd) {
	std::map<int, http::Parser*>::iterator it = _parsers.find(fd);

	if (it == _parsers.end()) {
		http::Parser* parser = new http::Parser();
		_parsers.insert(std::make_pair(fd, parser));
		return parser;
	}
	return it->second;
}

void Server::run() {
	struct sigaction sa;

	sa.sa_handler = handler::cgi::ProcessManager::sigchldHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) throw Exception("sigaction failed");
	_epollManager.init();

	for (std::map<int, config::Config>::iterator it = _configs.begin(); it != _configs.end();
		 ++it) {
		initServer(it->first);
	}
	loop();
}
// namespace server
