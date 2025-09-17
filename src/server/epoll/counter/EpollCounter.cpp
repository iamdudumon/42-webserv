// EpollCounter.cpp
#include "EpollCounter.hpp"

#include <algorithm>

#include "../../Defaults.hpp"

namespace server {
	EpollCounter::EpollCounter() : _count(0) {}

	bool EpollCounter::addFd(int fd) {
		if (_count >= defaults::EPOLL_SIZE) return false;
		_fds.push_back(fd);
		_count++;
		return true;
	}

	bool EpollCounter::deleteFd(int fd) {
		std::vector<int>::iterator it = std::find(_fds.begin(), _fds.end(), fd);
		if (it != _fds.end()) {
			_fds.erase(it);
			_count--;
			return true;
		}
		return false;
	}

	int EpollCounter::popFd() {
		int fd = _fds.front();
		_fds.erase(_fds.begin());
		_count--;
		return fd;
	}
}  // namespace server
