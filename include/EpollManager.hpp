#ifndef EPOLLMANAGER_HPP
#define EPOLLMANAGER_HPP

#include <vector>
#include <iostream>

#include <sys/epoll.h>

#include "Server.hpp"

class EpollManager {
	private:
		int _epollFd;
		const int _EPOLL_SIZE = 64;
		std::vector<struct epoll_event> _epollEvents;
		struct epoll_event _event;
		int	_eventCount;

	public:
		void	initEpoll(int serverSocket) {
			_epollFd = epoll_create(_EPOLL_SIZE);
			_epollEvents.resize(_EPOLL_SIZE);
			_event.events = EPOLLIN;
			_event.data.fd = serverSocket;
			epoll_ctl(_epollFd, EPOLL_CTL_ADD, serverSocket, &_event);
			
		}

		void	waitEvent() {
			if (_eventCount = (epoll_wait(_epollFd, (struct epoll_event *)_epollEvents.data(), _EPOLL_SIZE, -1)) == -1)
				std::cout << "error" << std::endl; // throw 변경
		}
};

#endif