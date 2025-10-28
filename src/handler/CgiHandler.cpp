#include "CgiHandler.hpp"

#include "cgi/Executor.hpp"
#include "utils/response.hpp"

using namespace handler;

CgiHandler::CgiHandler() {}

CgiHandler::~CgiHandler() {}

void CgiHandler::handle(int clientFd, const http::Packet& req,
						const router::RouteDecision& decision, server::EpollManager& epollManager) {
	cgi::Executor cgiExecutor;
	cgiExecutor.execute(decision, req, epollManager, _cgiProcessManager, clientFd);
}

void CgiHandler::handleEvent(int fd, server::EpollManager& epollManager) {
	_cgiProcessManager.handleCgiEvent(fd, epollManager);
}

int CgiHandler::getClientFd(int fd) const {
	return _cgiProcessManager.getClientFd(fd);
}

bool CgiHandler::isCgiProcess(int fd) const {
	return _cgiProcessManager.isCgiProcess(fd);
}

bool CgiHandler::isCgiProcessing(int clientFd) const {
	return _cgiProcessManager.isProcessing(clientFd);
}

bool CgiHandler::isCgiCompleted(int fd) const {
	return _cgiProcessManager.isCompleted(fd);
}

void CgiHandler::removeCgiProcess(int fd) {
	_cgiProcessManager.removeCgiProcess(fd);
}

std::string CgiHandler::getCgiResponse(int fd, const std::map<int, config::Config>& configs) {
	(void) configs;
	try {
		std::string cgiOutput = _cgiProcessManager.getResponse(fd);
		return utils::makeCgiResponse(cgiOutput);
	} catch (const handler::Exception&) {
		return utils::makeErrorResponse();
	}
}
