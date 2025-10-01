// CgiBuilder.hpp
#ifndef HANDLER_BUILDER_CGI_HPP
#define HANDLER_BUILDER_CGI_HPP

#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <map>
#include <string>

#include "../../../config/model/Config.hpp"
#include "../../../http/model/Packet.hpp"
#include "../../../router/model/RouteDecision.hpp"
#include "../../../server/epoll/manager/EpollManager.hpp"
#include "../../exception/Exception.hpp"
#include "../../utils/response.hpp"
#include "../Builder.hpp"

namespace handler {
	namespace builder {
		namespace cgi {
			struct Process {
					pid_t pid;
					int read_fd;
					std::string output;
					bool completed;

					Process() : pid(-1), read_fd(-1), completed(false) {}
			};

			class Builder {
				private:
					std::vector<std::string> _env_strings;
					std::vector<char*> _envp;
					std::vector<char*> _argv;

					// Static storage for active CGI processes
					static std::map<int, Process> _active_processes;  // fd -> CgiProcess

					void setArgv(const router::RouteDecision&);
					void setEnvp(const router::RouteDecision&, const http::Packet&,
								 const std::vector<config::Config>&);
					std::string readPipe(int pipefd);

				public:
					Builder() {}
					~Builder() {}
					http::Packet build(const router::RouteDecision&, const http::Packet&,
									   const std::vector<config::Config>&,
									   server::EpollManager& epoll_manager);

					// Static methods for CGI process management
					static void handleCgiEvent(int fd, server::EpollManager& epoll_manager);
					static bool hasCgiProcess(int fd);
					static std::string getCgiOutput(int fd);
			};
		}  // namespace Cgi
	}  // namespace builder
}  // namespace handler

#endif
