// ProcessManager.cpp
#include "ProcessManager.hpp"

#include <signal.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include <cerrno>

using namespace handler::cgi;

namespace {
	const unsigned int kStdoutEvents = EPOLLIN | EPOLLRDHUP | EPOLLET;
	const unsigned int kStdinEvents = EPOLLOUT | EPOLLET;
}

void ProcessManager::closeFdQuiet(int fd) {
	if (fd >= 0) close(fd);
}

void ProcessManager::detachStdout(Process& process, server::EpollManager& epollManager) {
	int fd = process.stdoutFd;
	if (fd < 0) return;

	process.stdoutFd = -1;
	if (process.stdoutRegistered) {
		epollManager.remove(fd);
		process.stdoutRegistered = false;
	} else
		closeFdQuiet(fd);
	process.stdoutClosed = true;
	process.completed = true;
}

void ProcessManager::detachStdin(Process& process, server::EpollManager& epollManager) {
	int fd = process.stdinFd;
	if (fd < 0) return;

	process.stdinFd = -1;
	_stdinToStdout.erase(fd);
	if (process.stdinRegistered) {
		epollManager.remove(fd);
		process.stdinRegistered = false;
	} else
		closeFdQuiet(fd);
}

void ProcessManager::trySendPendingInput(Process& process, server::EpollManager& epollManager) {
	if (process.stdinFd < 0) return;
	if (process.inputOffset >= process.input.size()) {
		detachStdin(process, epollManager);
		return;
	}

	while (process.inputOffset < process.input.size()) {
		ssize_t written = write(process.stdinFd, process.input.data() + process.inputOffset,
								process.input.size() - process.inputOffset);
		if (written > 0) {
			process.inputOffset += static_cast<size_t>(written);
			continue;
		}
		if (written == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
			if (!process.stdinRegistered) {
				epollManager.add(process.stdinFd, kStdinEvents);
				process.stdinRegistered = true;
			}
			return;
		}
		if (written == -1 && errno == EINTR) continue;
		// unrecoverable error
		detachStdin(process, epollManager);
		return;
	}
	detachStdin(process, epollManager);
}

void ProcessManager::handleStdoutEvent(Process& process, uint32_t events,
									   server::EpollManager& epollManager) {
	if (events & EPOLLIN) {
		char buffer[4096];
		while (true) {
			ssize_t n = read(process.stdoutFd, buffer, sizeof(buffer));
			if (n > 0) {
				process.output.append(buffer, n);
				continue;
			}
			if (n == 0) {
				process.stdoutClosed = true;
				break;
			}
			if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) break;
			if (n == -1 && errno == EINTR) continue;
			process.stdoutClosed = true;
			break;
		}
	}
	if (events & EPOLLRDHUP) process.stdoutClosed = true;
	if (process.stdoutClosed) detachStdout(process, epollManager);
}

void ProcessManager::handleCgiEvent(int fd, uint32_t events, server::EpollManager& epollManager) {
	std::map<int, Process>::iterator it = _processes.find(fd);
	if (it != _processes.end()) {
		handleStdoutEvent(it->second, events, epollManager);
		return;
	}

	std::map<int, int>::iterator stdinIt = _stdinToStdout.find(fd);
	if (stdinIt == _stdinToStdout.end()) return;
	std::map<int, Process>::iterator procIt = _processes.find(stdinIt->second);
	if (procIt == _processes.end()) return;
	if (events & EPOLLOUT) trySendPendingInput(procIt->second, epollManager);
}

void ProcessManager::registerCgiProcess(pid_t pid, int stdoutFd, int stdinFd, int clientFd,
										const std::string& body,
										server::EpollManager& epollManager) {
	Process process(pid, stdoutFd, stdinFd, clientFd, body);
	_processes[stdoutFd] = process;
	_clientToStdout[clientFd] = stdoutFd;
	if (stdinFd >= 0) _stdinToStdout[stdinFd] = stdoutFd;

	epollManager.add(stdoutFd, kStdoutEvents);
	Process& stored = _processes[stdoutFd];
	stored.stdoutRegistered = true;
	if (!stored.input.empty())
		trySendPendingInput(stored, epollManager);
	else
		detachStdin(stored, epollManager);
}

void ProcessManager::removeCgiProcess(int clientFd, server::EpollManager& epollManager) {
	std::map<int, int>::iterator it = _clientToStdout.find(clientFd);
	if (it == _clientToStdout.end()) return;
	int stdoutFd = it->second;
	_clientToStdout.erase(it);

	std::map<int, Process>::iterator procIt = _processes.find(stdoutFd);
	if (procIt == _processes.end()) return;
	Process& process = procIt->second;

	detachStdin(process, epollManager);
	detachStdout(process, epollManager);
	_processes.erase(procIt);
}

int ProcessManager::getClientFd(int fd) const {
	std::map<int, Process>::const_iterator it = _processes.find(fd);
	if (it != _processes.end()) return it->second.clientFd;
	std::map<int, int>::const_iterator stdinIt = _stdinToStdout.find(fd);
	if (stdinIt != _stdinToStdout.end()) {
		std::map<int, Process>::const_iterator proc = _processes.find(stdinIt->second);
		if (proc != _processes.end()) return proc->second.clientFd;
	}
	return -1;
}

std::string ProcessManager::getResponse(int stdoutFd) {
	std::map<int, Process>::iterator procIt = _processes.find(stdoutFd);
	return procIt->second.output;
}

bool ProcessManager::isCgiProcess(int fd) const {
	if (_processes.find(fd) != _processes.end()) return true;
	return _stdinToStdout.find(fd) != _stdinToStdout.end();
}

bool ProcessManager::isProcessing(int clientFd) const {
	return _clientToStdout.find(clientFd) != _clientToStdout.end();
}

bool ProcessManager::isCompleted(int clientFd) const {
	std::map<int, int>::const_iterator it = _clientToStdout.find(clientFd);
	if (it == _clientToStdout.end()) return false;
	std::map<int, Process>::const_iterator procIt = _processes.find(it->second);
	if (procIt == _processes.end()) return false;
	return procIt->second.completed;
}

bool ProcessManager::isStdout(int fd) const {
	return _processes.find(fd) != _processes.end();
}
