// RequestHandler.cpp
#include "RequestHandler.hpp"

#include <map>

#include "builder/Builder.hpp"

using namespace handler;

RequestHandler::RequestHandler() : _defaultBuilder(NULL) {
	_builders[router::RouteDecision::ServeFile] = new builder::FileBuilder();
	_builders[router::RouteDecision::ServeAutoIndex] = new builder::AutoIndexBuilder();
	_builders[router::RouteDecision::Redirect] = new builder::RedirectBuilder();
	_builders[router::RouteDecision::Error] = new builder::ErrorBuilder();
	_defaultBuilder = _builders[router::RouteDecision::Error];
}

RequestHandler::~RequestHandler() {
	for (std::map<router::RouteDecision::Action, builder::IBuilder*>::iterator it =
			 _builders.begin();
		 it != _builders.end(); ++it) {
		if (it->second && it->second != _defaultBuilder) delete it->second;
	}
	if (_defaultBuilder) delete _defaultBuilder;
}

const builder::IBuilder* RequestHandler::selectBuilder(router::RouteDecision::Action action) const {
	std::map<router::RouteDecision::Action, builder::IBuilder*>::const_iterator it =
		_builders.find(action);

	if (it != _builders.end() && it->second) return it->second;
	return _defaultBuilder;
}

http::Packet RequestHandler::handle(const http::Packet& req,
									const std::vector<config::Config>& configs,
									int localPort) const {
	router::RouteDecision decision = _router.route(req, configs, localPort);
	const builder::IBuilder* builder = selectBuilder(decision.action);

	return builder->build(decision, req, configs);
}

void RequestHandler::handleCgi(const http::Packet& req, const std::vector<config::Config>& configs,
							   int localPort, int client_fd, server::EpollManager& epollManager) {
	router::RouteDecision decision = _router.route(req, configs, localPort);
	cgi::Builder cgiBuilder;
	cgiBuilder.build(decision, req, epollManager, _cgiManager, client_fd);
}

void RequestHandler::handleCgiEvent(int fd, server::EpollManager& epollManager) {
	_cgiManager.handleCgiEvent(fd, epollManager);
}

int RequestHandler::getClientFd(int fd) const {
	return _cgiManager.getClientFd(fd);
}

bool RequestHandler::isCgiProcess(int fd) const {
	return _cgiManager.isCgiProcess(fd);
}

bool RequestHandler::isCgiProcessing(int clientFd) const {
	return _cgiManager.isProcessing(clientFd);
}

bool RequestHandler::isCgiCompleted(int fd) const {
	return _cgiManager.isCompleted(fd);
}

void RequestHandler::removeCgiProcess(int fd) {
	_cgiManager.removeCgiProcess(fd);
}

std::string RequestHandler::getCgiResponse(int fd) {
	try {
		return utils::makeCgiResponse(_cgiManager.getResponse(fd));
	} catch (...) {
		return utils::makeCgiErrorResponse(http::StatusCode::InternalServerError);
	}
}

router::RouteDecision RequestHandler::route(const http::Packet& req,
											const std::vector<config::Config>& configs,
											int localPort) const {
	return _router.route(req, configs, localPort);
}