#include "Manager.hpp"

using namespace handler::cgi;

void Manager::sigchld_handler(int sig) {
	(void) sig;
	int status;
	// 모든 종료된 자식 프로세스 회수
	while (waitpid(-1, &status, WNOHANG) > 0) {
	}
}

int Manager::getClientFd(int cgi_fd) const {
	std::map<int, Process>::const_iterator it = _active_processes.find(cgi_fd);
	if (it == _active_processes.end()) {
		return -1; // 해당 cgi_fd에 대한 프로세스가 없음
	}
	return it->second.client_fd;
}

void Manager::registerProcess(pid_t pid, int cgi_fd, int client_fd) {
	Process process;
	process.pid = pid;
	process.cgi_fd = cgi_fd;
	process.client_fd = client_fd;
	process.completed = false;
	process.output.clear();

	_active_processes[cgi_fd] = process;
	_clientToCgi[client_fd] = cgi_fd;
}

void Manager::handleCgiEvent(int cgi_fd, server::EpollManager& epoll_manager) {
	std::map<int, Process>::iterator it = _active_processes.find(cgi_fd);
	if (it == _active_processes.end()) {
		return;	 // 프로세스를 찾을 수 없음
	}
	Process& process = it->second;
	char buffer[4096];
	int n;
	while ((n = read(cgi_fd, buffer, sizeof(buffer) - 1)) > 0) {
		buffer[n] = '\0';
		process.output += std::string(buffer);
	}
	process.completed = true;
	epoll_manager.remove(cgi_fd);
	close(cgi_fd);
}

bool Manager::isCgiProcess(int fd) const {
	return _active_processes.find(fd) != _active_processes.end();
}

bool Manager::isCompleted(int fd) const {
	std::map<int, int>::const_iterator it = _clientToCgi.find(fd);
	if (it == _clientToCgi.end()) {
		return false;
	}

	int cgi_fd = it->second;
	std::map<int, Process>::const_iterator cgi_it = _active_processes.find(cgi_fd);
	if (cgi_it == _active_processes.end()) {
		return false;
	}
	return cgi_it->second.completed;
}

void Manager::removeCgiProcess(int client_fd) {
	std::map<int, int>::iterator it = _clientToCgi.find(client_fd);
	if (it != _clientToCgi.end()) {
		int cgi_fd = it->second;
		_clientToCgi.erase(it);

		std::map<int, Process>::iterator proc_it = _active_processes.find(cgi_fd);
		if (proc_it != _active_processes.end()) {
			_active_processes.erase(proc_it);
		}
	}
}

std::string Manager::getResponse(int client_fd) {
	std::map<int, int>::iterator it = _clientToCgi.find(client_fd);
	if (it == _clientToCgi.end()) {
		throw Exception(); // 클라이언트에 해당하는 CGI 프로세스가 없음
	}
	int cgi_fd = it->second;
	std::map<int, Process>::iterator proc_it = _active_processes.find(cgi_fd);
	if (proc_it == _active_processes.end()) {
		throw Exception(); // CGI 프로세스를 찾을 수 없음
	}
	std::string output = proc_it->second.output;
	_active_processes.erase(proc_it);
	_clientToCgi.erase(it);
	if (output.empty()) {
		throw Exception(); // CGI 출력이 비어 있음
	}
	return output;
}