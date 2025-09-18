// SocketWrapper.hpp
#ifndef SERVER_SOCKETWRAPPER_HPP
#define SERVER_SOCKETWRAPPER_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../exception/Exception.hpp"

namespace server {
	namespace socket {
		inline int create(int domain, int type, int protocol) {
			int fd = ::socket(domain, type, protocol);
			if (fd < 0) throw Exception("socket");
			return fd;
		}

		inline void bind(int fd, const sockaddr* address, socklen_t addressLen) {
			if (::bind(fd, address, addressLen) < 0) throw Exception("bind");
		}

		inline void listen(int fd, int backlog) {
			if (::listen(fd, backlog) < 0) throw Exception("listen");
		}

		inline int accept(int fd, sockaddr* address, socklen_t* addressLen) {
			int client = ::accept(fd, address, addressLen);
			if (client < 0) throw Exception("accept");
			return client;
		}

		inline void setOption(int fd, int level, int optionName, const void* optionValue,
							  socklen_t optionLen) {
			if (::setsockopt(fd, level, optionName, optionValue, optionLen) < 0)
				throw Exception("setsockopt");
		}
	}  // namespace socket
}  // namespace server

#endif
