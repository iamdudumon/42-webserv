// ProcessManager.hpp
#ifndef HANDLER_CGI_PROCESS_MANAGER_HPP
#define HANDLER_CGI_PROCESS_MANAGER_HPP

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <map>
#include <string>

#include "../../http/model/Packet.hpp"
#include "../../server/epoll/manager/EpollManager.hpp"
#include "../exception/Exception.hpp"

namespace handler {
	namespace cgi {
		class ProcessManager {
			private:
				struct Process {
						pid_t pid;
						int stdoutFd;
						int stdinFd;
						int clientFd;
						std::string output;
						std::string input;
						size_t inputOffset;
						bool stdoutClosed;
						bool stdinRegistered;
						bool stdoutRegistered;
						bool completed;

						Process() :
							pid(-1),
							stdoutFd(-1),
							stdinFd(-1),
							clientFd(-1),
							inputOffset(0),
							stdoutClosed(false),
							stdinRegistered(false),
							stdoutRegistered(false),
							completed(false) {}

						Process(pid_t p, int outFd, int inFd, int clFd, const std::string& body) :
							pid(p),
							stdoutFd(outFd),
							stdinFd(inFd),
							clientFd(clFd),
							input(body),
							inputOffset(0),
							stdoutClosed(false),
							stdinRegistered(false),
							stdoutRegistered(false),
							completed(false) {}
				};

				std::map<int, Process> _processes;
				std::map<int, int> _stdinToStdout;
				std::map<int, int> _clientToStdout;

				void closeFdQuiet(int);
				void detachStdout(Process&, server::EpollManager&);
				void detachStdin(Process&, server::EpollManager&);
				void trySendPendingInput(Process&, server::EpollManager&);
				void handleStdoutEvent(Process&, uint32_t, server::EpollManager&);

			public:
				ProcessManager() {}
				~ProcessManager() {}

				void handleCgiEvent(int, uint32_t, server::EpollManager&);
				void registerCgiProcess(pid_t, int, int, int, const std::string&,
										server::EpollManager&);
				void removeCgiProcess(int, server::EpollManager&);
				int getClientFd(int) const;
				std::string getResponse(int);
				bool isCgiProcess(int) const;
				bool isProcessing(int) const;
				bool isCompleted(int) const;
				bool isStdout(int) const;
		};
	}  // namespace cgi
}  // namespace handler

#endif
