#ifndef HANDLER_BUILDER_CGI_MANAGER_HPP
#define HANDLER_BUILDER_CGI_MANAGER_HPP

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <map>
#include <string>

#include "../../http/model/Packet.hpp"
#include "../../server/epoll/manager/EpollManager.hpp"
#include "../exception/Exception.hpp"
#include "../utils/response.hpp"

namespace handler {
	namespace cgi {
		class Manager {
			private:
				struct Process {
						pid_t pid;
						int cgi_fd;		// CGI 프로세스의 출력 파이프
						int client_fd;	// 클라이언트 소켓 파일 디스크립터
						std::string output;
						bool completed;
						Process() : pid(-1), cgi_fd(-1), client_fd(-1), completed(false) {}
				};
				std::map<int, Process> _active_processes;  // cgi fd -> Process
				std::map<int, int> _clientToCgi;		   // client fd -> cgi fd
			public:
				Manager() {}
				~Manager() {}

				static void sigchld_handler(int);				  // SIGCHLD 핸들러
				int getClientFd(int cgi_fd) const;				  // cgi_fd로 client_fd 얻기
				void registerProcess(pid_t, int, int);			  // CGI 프로세스 등록
				void handleCgiEvent(int, server::EpollManager&);  // CGI 이벤트 처리
				bool isCgiProcess(int) const;					  // fd가 CGI 프로세스인지 확인
				bool isCompleted(int) const;   // fd가 CGI 프로세스의 출력을 완료했는지 확인
				void removeCgiProcess(int);	   // CGI 프로세스 제거
				std::string getResponse(int);  // CGI 출력 가져오기
		};
	}  // namespace cgi
}  // namespace handler

#endif