#include "Server.hpp"

#include <arpa/inet.h>
#include <dirent.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <cstring>
#include <fstream>
#include <sstream>

#include "../router/Router.hpp"

Server::Server(std::vector<Config> configs) :
	_configs(configs), _socketOption(1), _addressSize(sizeof(_serverAddress)) {
	_epollManager.initEpoll();
}

void Server::initAddress(int index) {
	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_port = htons(_configs[index].getListen());
	_serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	std::fill(_serverAddress.sin_zero, _serverAddress.sin_zero + 8, 0);
}

void Server::initServer() {
	int serverSocket = SocketWrapper::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	SocketWrapper::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &_socketOption,
							  sizeof(_socketOption));
	SocketWrapper::bind(serverSocket, (struct sockaddr*) &_serverAddress, sizeof(_serverAddress));
	SocketWrapper::listen(serverSocket, 10);
	_serverSockets.insert(serverSocket);
	_epollManager.addEpollFd(serverSocket);
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
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	} catch (...) {
		std::cout << "[Error] Unknown Error" << std::endl;
	}
}

void Server::handleEvents() {
	for (int i = 0; i < _epollManager.getEventCount(); i++) {
		if (_serverSockets.find(_epollManager.getEpollEventsAt(i).data.fd) !=
			_serverSockets.end()) {
			_clientSocket = SocketWrapper::accept(*_serverSockets.find(
													  _epollManager.getEpollEventsAt(i).data.fd),
												  (struct sockaddr*) &_clientAddress,
												  (socklen_t*) &_addressSize);
			std::cout << "new client :" << _clientSocket << std::endl;
			_epollManager.addEpollFd(_clientSocket);
		} else {
			int clientFd = _epollManager.getEpollEventsAt(i).data.fd;
			std::string buffer = readSocket(clientFd);
			if (!buffer.size() ||
				_epollManager.getEpollEventsAt(i).events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
				_epollManager.deleteEpollFd(clientFd);
			} else {
				HttpPacket httpRequest = convertHttpPacket(buffer);

				// 로컬 포트 확인
				int localPort = 0;
				struct sockaddr_in addr;
				socklen_t len = sizeof(addr);
				if (getsockname(clientFd, (struct sockaddr*) &addr, &len) == 0) {
					localPort = ntohs(addr.sin_port);
				}

				// Router 호출
				Router router;
				RouteDecision d = router.route(httpRequest, _configs, localPort);

				// 공통 상태라인
				HTTP::StatusLine statusLine = {"HTTP/1.1", d.status,
											   HTTP::StatusCode::to_reasonPhrase(d.status)};
				HttpPacket httpResponse(statusLine, Header(), Body());

				// Allow 헤더 문자열 조립 헬퍼
				std::string allowStr;
				for (size_t k = 0; k < d.allow_methods.size(); ++k) {
					if (k) allowStr += ", ";
					allowStr += d.allow_methods[k];
				}

				// 파일 읽기 헬퍼
				std::string fileData;
				bool fileOk = false;
				if (d.action == RouteDecision::ServeFile) {
					std::ifstream ifs(d.fs_path.c_str(), std::ios::in | std::ios::binary);
					if (ifs.is_open()) {
						std::ostringstream ss;
						ss << ifs.rdbuf();
						fileData = ss.str();
						fileOk = true;
					}
				}

				switch (d.action) {
					case RouteDecision::ServeFile: {
						if (!fileOk) {
							// 파일 열기 실패 시 404
							httpResponse = HttpPacket({"HTTP/1.1", HTTP::StatusCode::NotFound,
													   HTTP::StatusCode::to_reasonPhrase(
														   HTTP::StatusCode::NotFound)},
													  Header(), Body());
							httpResponse.addHeader("Content-Type", "text/plain");
							const char* msg = "Not Found";
							httpResponse.appendBody(msg, 9);
							break;
						}
						httpResponse.addHeader("Content-Type", d.content_type_hint.empty()
																   ? "application/octet-stream"
																   : d.content_type_hint);
						if (!fileData.empty())
							httpResponse.appendBody(fileData.data(), fileData.size());
						break;
					}
					case RouteDecision::ServeAutoIndex: {
						// 단순 autoindex HTML 생성
						std::ostringstream ss;
						ss << "<html><body><h1>Index of " << d.fs_path << "</h1><ul>";
						DIR* dp = opendir(d.fs_path.c_str());
						if (dp) {
							struct dirent* ent;
							while ((ent = readdir(dp)) != NULL) {
								std::string name = ent->d_name;
								if (name == "." || name == "..") continue;
								ss << "<li>" << name << "</li>";
							}
							closedir(dp);
						}
						ss << "</ul></body></html>";
						std::string html = ss.str();
						httpResponse.addHeader("Content-Type", "text/html");
						httpResponse.appendBody(html.c_str(), html.size());
						break;
					}
					case RouteDecision::Redirect: {
						httpResponse.addHeader("Location", d.redirect_location);
						std::string msg = std::string("Redirecting to ") + d.redirect_location;
						httpResponse.addHeader("Content-Type", "text/plain");
						httpResponse.appendBody(msg.c_str(), msg.size());
						break;
					}
					case RouteDecision::Cgi: {
						// 미구현: 임시 메시지
						httpResponse.addHeader("Content-Type", "text/plain");
						const char* msg = "CGI not implemented";
						httpResponse.appendBody(msg, 20);
						break;
					}
					case RouteDecision::Error:
					default: {
						if (d.status == HTTP::StatusCode::MethodNotAllowed && !allowStr.empty()) {
							httpResponse.addHeader("Allow", allowStr);
						}
						const char* reason = HTTP::StatusCode::to_reasonPhrase(d.status);
						httpResponse.addHeader("Content-Type", "text/plain");
						httpResponse.appendBody(reason, std::strlen(reason));
						break;
					}
				}

				writeHttpPacket(clientFd, httpResponse);
			}
		}
	}
}

std::string Server::readSocket(int socketFd) {
	char buffer[SystemConfig::Size::BUFFER_SIZE] = {0};
	int readSize;
	std::string request = "";

	while ((readSize = read(socketFd, buffer, SystemConfig::Size::BUFFER_SIZE)) ==
		   SystemConfig::Size::BUFFER_SIZE) {
		request.append(buffer);
	}
	request.append(buffer);
	return request;
}

void Server::writeHttpPacket(int socketFd, HttpPacket httpResponse) {
	std::string rawResponse = HttpSerializer::serialize(httpResponse);
	std::cout << rawResponse << "\n";
	writeSocket(socketFd, rawResponse);
}

HttpPacket Server::convertHttpPacket(std::string& buffer) {
	HttpParser httpParser(buffer);
	httpParser.parse();
	HttpPacket httpRequest = httpParser.getResult();
	return httpRequest;
}

void Server::writeSocket(int socketFd, std::string rawData) {
	write(socketFd, rawData.c_str(), rawData.size());
}

void Server::runServer() {
	for (unsigned long i = 0; i < _configs.size(); i++) {
		initAddress(i);
		initServer();
	}
	loopServer();
}
