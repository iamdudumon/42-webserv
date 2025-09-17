// EpollCounter.hpp
#ifndef SERVER_EPOLL_COUNTER_HPP
#define SERVER_EPOLL_COUNTER_HPP

#include <algorithm>
#include <vector>

#include "../../../../include/SystemConfig.hpp"

namespace server {
	class EpollCounter {
		private:
			std::vector<int> _fds;
			int _count;

		public:
			EpollCounter();

			bool addFd(int);
			bool deleteFd(int);
			int popFd();
	};
}  // namespace server

#endif
