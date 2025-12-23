#include "ClientManager.hpp"

#include <ctime>

using namespace client;

ClientManager::ClientManager() {}

ClientManager::~ClientManager() {
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		delete it->second;
	}
	_clients.clear();
}

Client* ClientManager::getClient(int fd) {
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it == _clients.end()) return NULL;
	return it->second;
}

Client* ClientManager::ensureClient(int fd, const config::Config* config) {
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it == _clients.end()) {
		Client* client = new Client(fd, config);
		_clients.insert(std::make_pair(fd, client));
		return client;
	}
	return it->second;
}

void ClientManager::removeClient(int fd) {
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it != _clients.end()) {
		delete it->second;
		_clients.erase(it);
	}
}

std::vector<int> ClientManager::checkTimeouts() {
	std::vector<int> timedOutFds;
	time_t now = std::time(NULL);
	std::map<int, Client*>::iterator it = _clients.begin();

	while (it != _clients.end()) {
		if (it->second->isTimedOut(now)) {
			timedOutFds.push_back(it->first);
			delete it->second;
			_clients.erase(it++);
		} else {
			++it;
		}
	}
	return timedOutFds;
}
