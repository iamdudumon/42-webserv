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
	
		void initAddress(){
			_address.sin_family = AF_INET;
			_address.sin_port = htons(_PORT);
			_address.sin_addr.s_addr = htonl(INADDR_ANY);
		}

		int validateFunction(int returnValue){
			if (returnValue < 0)
				throw "error"; // 추후 exception 발생으로 수정
			return returnValue;
		}

		void initServer() {
			_serverSocket = validateFunction(socket(PF_INET, SOCK_STREAM, 0));
			validateFunction(setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &_socketOption, sizeof(_socketOption)));
			validateFunction(bind(_serverSocket, (struct sockaddr *)&_address, sizeof(_address)));
			validateFunction(listen(_serverSocket, 10));
		}

		void loopServer() {
			while(1){
				_clientSocket = validateFunction(accept(_serverSocket, (struct sockaddr *)&_address, (socklen_t *)&_addressSize));
				std::string request = readSocket();
				std::cout << request << std::endl; // testcode
				// 파싱후 정적페이지 전달 추가
			}
		}

		std::string readSocket() {
			char	buffer[30000] = {0}; // 추후 루프 혹은 멀티플렉싱 처리를 통해 긴 요청 응답 가능하게 변경
			int		readSize;
	
			readSize = read(_serverSocket, buffer, 30000);
			std::string request(buffer); // http 파싱 추가 예정
			return request;
		}

		/*void writeSocket(){

		}*/
	
	public:
		Server(Config config): _PORT(config.getListen()), _addressSize(sizeof(_address)){
			initAddress();
		}

		void runServer(){
			initServer();
			loopServer();
		}

};

#endif