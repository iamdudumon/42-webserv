#include "../include/Server.hpp"

Server::Server(Config config)
	: _PORT(config.getListen()),
		_socketOption(1),
		_addressSize(sizeof(_address)) {
	initAddress();
}

void Server::initAddress() {
	_address.sin_family = AF_INET;
	_address.sin_port = htons(_PORT);
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
	std::fill(_address.sin_zero, _address.sin_zero + 8, 0);
}

int Server::validateFunction(int returnValue) {
	if (returnValue < 0)
		std::cout << "error" << std::endl;//throw "error";	 // 추후 exception 발생으로 수정
	return returnValue;
}

void Server::initServer() {
	_serverSocket = validateFunction(socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0));
	validateFunction(setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR,
								&_socketOption, sizeof(_socketOption)));
	validateFunction(bind(_serverSocket, (struct sockaddr*) &_address,
							sizeof(_address)));
	validateFunction(listen(_serverSocket, 10));
	epollManager.initEpoll(_serverSocket);
}

void Server::loopServer() {
	while (1) {
		_clientSocket = validateFunction(
			accept(_serverSocket, (struct sockaddr*) &_address,
					(socklen_t*) &_addressSize));
		readHttpPacket();
		writeHttpPacket();
		// 파싱후 정적페이지 전달 추가
	}
}

std::string Server::readSocket() {
	char buffer[30000] = {0};  // 추후 루프 혹은 멀티플렉싱 처리를 통해 긴 요청 응답 가능하게 변경

	read(_clientSocket, buffer, 30000);
	std::string request(buffer);
	return request;
}

void Server::writeHttpPacket() {  // 메소드별 분할 해야함 (testcode)
	writeSocket("HTTP/1.1 200 OK\nContent-Length: 4\nContent-Type: text/html\n\ntest");
}

HttpRequest Server::readHttpPacket() {
	HttpRequest httpRequest = HttpParser::parseRequestHttp(readSocket());
	return httpRequest;
}

void Server::writeSocket(std::string rawData){
	write(_clientSocket, rawData.c_str(), rawData.size());
}

void Server::runServer() {
	initServer();
	loopServer();
}