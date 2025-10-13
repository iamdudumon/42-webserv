// CgiBuilder.hpp
#ifndef HANDLER_CGI_BUILDER_HPP
#define HANDLER_CGI_BUILDER_HPP

#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include <map>

#include "../../config/model/Config.hpp"
#include "../../http/model/Packet.hpp"
#include "../../router/model/RouteDecision.hpp"
#include "../../server/epoll/manager/EpollManager.hpp"
#include "../exception/Exception.hpp"
#include "../utils/response.hpp"
#include "Manager.hpp"

namespace handler {
	namespace cgi {
		class Builder {
			private:
				std::vector<std::string> _envStrings;
				std::vector<char*> _envp;
				std::vector<char*> _argv;

				void setArgv(const router::RouteDecision&);
				void setEnvp(const router::RouteDecision&, const http::Packet&);

			public:
				Builder() {}
				~Builder() {}
				void build(const router::RouteDecision&, const http::Packet&, server::EpollManager&,
						   Manager&, int);
		};
	}  // namespace cgi
}  // namespace handler

#endif
