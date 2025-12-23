// EventHandler.cpp
#include "EventHandler.hpp"

#include <cerrno>
#include <ctime>

#include "../client/exception/Exception.hpp"
#include "../http/Enums.hpp"
#include "../http/response/Factory.hpp"
#include "../http/response/Serializer.hpp"
#include "cgi/Executor.hpp"

using namespace handler;

namespace {
	bool shouldKeepAlive(const std::string& connectionHeader) {
		bool keepAlive = true;

		if (!connectionHeader.empty()) {
			if (connectionHeader == "close")
				keepAlive = false;
			else if (connectionHeader == "keep-alive")
				keepAlive = true;
		}
		return keepAlive;
	}
}

EventHandler::EventHandler() {}

EventHandler::~EventHandler() {}

EventResult EventHandler::handleEvent(int fd, uint32_t events, const config::Config* config,
									  server::EpollManager& epollManager) {
	if (_cgiProcessManager.isCgiProcess(fd))
		return handleCgiEvent(fd, events, config, epollManager);
	return handleClientEvent(fd, events, config, epollManager);
}

EventResult EventHandler::handleCgiEvent(int fd, uint32_t events, const config::Config* config,
										 server::EpollManager& epollManager) {
	int clientFd = _cgiProcessManager.getClientFd(fd);
	if (clientFd == -1) return EventResult();
	_cgiProcessManager.handleCgiEvent(fd, events, epollManager);
	if (!_cgiProcessManager.isStdout(fd) || !_cgiProcessManager.isCompleted(clientFd))
		return EventResult();

	bool keepAlive = false;
	client::Client* client = _clientManager.getClient(clientFd);
	if (client) {
		if (client->getCgiConfig()) config = client->getCgiConfig();
		keepAlive = client->isKeepAlive();
		client->setState(client::Client::Sending);
	}

	std::string cgiOutput = _cgiProcessManager.getResponse(fd);
	std::string rawResponse;

	try {
		if (cgiOutput.empty() || !config)
			rawResponse = http::response::Serializer::serialize(
				http::response::Factory::createError(http::StatusCode::InternalServerError, config),
				keepAlive);
		else
			rawResponse = _responseBuilder.buildCgi(cgiOutput, keepAlive);
	} catch (const std::exception&) {
		rawResponse = http::response::Serializer::serialize(
			http::response::Factory::createError(http::StatusCode::BadRequest, config), keepAlive);
	}
	_cgiProcessManager.removeCgiProcess(clientFd, epollManager);

	EventResult result;
	result.setRawResponse(clientFd, rawResponse, keepAlive);

	client = _clientManager.getClient(clientFd);
	client->setState(client->isKeepAlive() ? client::Client::Receiving : client::Client::Closing);
	return result;
}

EventResult EventHandler::handleClientEvent(int fd, uint32_t events, const config::Config* config,
											server::EpollManager& epollManager) {
	EventResult result;
	bool disconnected = (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) != 0;
	bool peerClosed = false;
	client::Client* client = _clientManager.ensureClient(fd, config);
	client->updateLastActivity();

	if (events & EPOLLIN) {
		std::string buffer = client->readSocket(peerClosed);
		try {
			client->processData(buffer);
		} catch (const client::Exception& e) {
			handleParseError(client, config, e.getErrorCode(), e.getErrorMessage());
		}
	}

	if (peerClosed) disconnected = true;
	if (disconnected) client->markEndOfInput();

	if (client->getState() == client::Client::Receiving) {
		if (client->hasRequest()) {
			http::Packet* request = client->takeRequest();
			processRequest(client, config, epollManager, request);
			delete request;
		}
	}

	if (disconnected) client->setState(client::Client::Closing);
	if (client->getState() == client::Client::Sending && client->getResponse()) {
		result.setPacketResponse(fd, *client->getResponse(), client->isKeepAlive());
		if (client->isKeepAlive() && !disconnected) {
			client->clearResponse();
			client->setState(client::Client::Receiving);

			if (client->hasUnconsumedInput()) {
				try {
					client->processData("");
					if (client->hasRequest()) {
						http::Packet* request = client->takeRequest();
						processRequest(client, config, epollManager, request);
						delete request;
					}
				} catch (const client::Exception& e) {
					handleParseError(client, config, e.getErrorCode(), e.getErrorMessage());
				}
			}

			epollManager.modify(fd, client->hasUnconsumedInput() || client->hasRequest()
										? (EPOLLIN | EPOLLOUT)
										: EPOLLIN);
		} else
			client->setState(client::Client::Closing);
	} else if (client->getState() == client::Client::Closing) {
		if (client->getResponse())
			result.setPacketResponse(fd, *client->getResponse(), false);
		else
			result.reset(fd);
	}
	return result;
}

void EventHandler::handleParseError(client::Client* client, const config::Config* config,
									http::StatusCode::Value errorCode,
									const std::string& errorMessage) {
	const std::string fallbackContentType =
		!errorMessage.empty() ? http::ContentType::to_string(http::ContentType::CONTENT_TEXT_PLAIN)
							  : std::string();
	http::Packet errorPacket =
		http::response::Factory::createError(errorCode, config, errorMessage, fallbackContentType);

	client->setResponse(new http::Packet(errorPacket));
	client->setState(client::Client::Closing);
}

void EventHandler::processRequest(client::Client* client, const config::Config* config,
								  server::EpollManager& epollManager,
								  const http::Packet* httpRequest) {
	if (!httpRequest) return;
	bool keepAlive = shouldKeepAlive(httpRequest->getHeader().get("Connection"));
	router::RouteDecision decision = _router.route(*httpRequest, *config);
	if (decision.action == router::RouteDecision::Cgi) {
		cgi::Executor executor;
		executor.execute(decision, *httpRequest, epollManager, _cgiProcessManager, client->getFd());
		client->setCgiConfig(decision.server);
		client->setKeepAlive(keepAlive);
		client->setState(client::Client::Processing);
		return;
	}

	http::Packet httpResponse = _responseBuilder.build(*httpRequest, decision, *config);
	client->setResponse(new http::Packet(httpResponse));
	client->setKeepAlive(keepAlive);
	client->setState(client::Client::Sending);
}

void EventHandler::checkTimeouts(server::EpollManager& epollManager) {
	std::vector<int> deadFds = _clientManager.checkTimeouts();

	for (std::vector<int>::iterator it = deadFds.begin(); it != deadFds.end(); ++it) {
		_cgiProcessManager.removeCgiProcess(*it, epollManager);
		epollManager.remove(*it);
	}
}

void EventHandler::cleanup(int fd, server::EpollManager& epollManager) {
	_clientManager.removeClient(fd);
	_cgiProcessManager.removeCgiProcess(fd, epollManager);
}
