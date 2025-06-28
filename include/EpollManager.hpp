#ifndef EPOLLMANAGER_HPP
#define EPOLLMANAGER_HPP

#include <vector>
#include <iostream>

#include <sys/epoll.h>

#include "Server.hpp"

class EpollManager {
	private:
		int _epollFd;
		static const int	EPOLL_SIZE = 64;
		std::vector<struct epoll_event> _epollEvents;
		struct epoll_event _event;
		int	_eventCount;

	public:
		void	initEpoll(int serverSocket) {
			_epollFd = epoll_create(EPOLL_SIZE);
			_epollEvents.resize(EPOLL_SIZE);
			addEpollFd(serverSocket);
		}

		void	addEpollFd(int socketFd) {
			_event.events = EPOLLIN;
			_event.data.fd = socketFd;
			epoll_ctl(_epollFd, EPOLL_CTL_ADD, socketFd, &_event);
		}

		void	deleteEpollFd(int socketFd) {
			epoll_ctl(_epollFd, EPOLL_CTL_DEL, socketFd, NULL);
			close(socketFd);
		}

		void	waitEvent() {
			if ((_eventCount = (epoll_wait(_epollFd, (struct epoll_event *)_epollEvents.data(), EPOLL_SIZE, -1))) == -1)
				std::cout << "error" << std::endl; // throw 변경
		}

		int	getEpollFd() {
			return _epollFd;
		}

		int	getEventCount() {
			return _eventCount;
		}

		int	getEpollEventsFdAt(int index) {
			if (index < 0 || static_cast<int>(_epollEvents.size()) <= index) {
				std::cout << "error" << std::endl; // throw 변경
			}
			return _epollEvents[index].data.fd;
		}
};

#endif