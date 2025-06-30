#ifndef EPOLLCOUNTER_HPP
#define EPOLLCOUNTER_HPP

#include <vector>
#include <algorithm>

#include "EpollManager.hpp"

class EpollCounter {
	private:
		std::vector<int>	_epollFdVector;
		int					_epollCount;
		int					_epollMaxCount;
	public:
		EpollCounter(int epollMaxCount): _epollMaxCount(epollMaxCount) {
			_epollCount = 0;
		}
		
		bool	addFd(int fd) {
			if (_epollCount >= _epollMaxCount)
				return false;
			_epollFdVector.push_back(fd);
			_epollCount++;
			return true;
		}

		bool	deleteFd(int fd) {
			if (std::find(_epollFdVector.begin(), _epollFdVector.end(), fd) != _epollFdVector.end()) {
				_epollFdVector.erase(std::find(_epollFdVector.begin(), _epollFdVector.end(), fd));
				_epollCount--;
				return true;
			}
			return false;
		}

		int		popFd() {
			int	fd;

			fd = _epollFdVector.front();
			_epollFdVector.erase(_epollFdVector.begin());
			_epollCount--;
			return fd;
		}
		
};

#endif