// MainHandler.cpp
#include "MainHandler.hpp"

#include "utils/response.hpp"

using namespace handler;

bool MainHandler::handle(int clientFd, const http::Packet& request, const config::Config& config,
						 server::EpollManager& epollManager, http::Packet& response) {
	router::RouteDecision decision = _router.route(request, config);
	if (decision.action == router::RouteDecision::Cgi) {
		_cgiHandler.handle(clientFd, request, decision, epollManager);
		return false;
	}

	if (!decision.server) {
		response = utils::makePlainResponse(
			http::StatusCode::InternalServerError,
			http::StatusCode::to_reasonPhrase(http::StatusCode::InternalServerError),
			http::ContentType::to_string(http::ContentType::CONTENT_TEXT_PLAIN));
		return true;
	}

	response = _requestHandler.handle(clientFd, request, decision, *decision.server);
	return true;
}

void MainHandler::handleCgiEvent(int fd, server::EpollManager& epollManager) {
	_cgiHandler.handleEvent(fd, epollManager);
}

int MainHandler::getClientFd(int fd) const {
	return _cgiHandler.getClientFd(fd);
}

bool MainHandler::isCgiProcess(int fd) const {
	return _cgiHandler.isCgiProcess(fd);
}

bool MainHandler::isCgiProcessing(int clientFd) const {
	return _cgiHandler.isCgiProcessing(clientFd);
}

bool MainHandler::isCgiCompleted(int fd) const {
	return _cgiHandler.isCgiCompleted(fd);
}

void MainHandler::removeCgiProcess(int fd) {
	_cgiHandler.removeCgiProcess(fd);
}

std::string MainHandler::getCgiResponse(int fd, const config::Config& config) {
	return _cgiHandler.getCgiResponse(fd, config);
}
