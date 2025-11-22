// EventHandler.cpp
#include "EventHandler.hpp"

#include <unistd.h>

#include <cerrno>

#include "../http/Enums.hpp"
#include "../http/response/Factory.hpp"
#include "../http/response/Serializer.hpp"
#include "../server/Defaults.hpp"
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

EventHandler::~EventHandler() {
	for (std::map<int, client::Client*>::iterator it = _clients.begin(); it != _clients.end();
		 ++it) {
		delete it->second;
	}
	_clients.clear();
}

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
	std::map<int, client::Client*>::iterator it = _clients.find(clientFd);
	if (it != _clients.end()) {
		if (it->second->getCgiConfig()) config = it->second->getCgiConfig();
		keepAlive = it->second->isKeepAlive();
		it->second->setState(client::Client::Sending);
		// CGI context is part of client, no need to erase explicitly unless we want to reset it
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
	return result;
}

EventResult EventHandler::handleClientEvent(int fd, uint32_t events, const config::Config* config,
											server::EpollManager& epollManager) {
	EventResult result;
	bool disconnected = (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) != 0;
	bool peerClosed = false;

	client::Client* client = ensureClient(fd, config);
	if (client->getState() == client::Client::None) client->setState(client::Client::Receiving);

	if (client->getState() == client::Client::Receiving) {
		std::string buffer = readSocket(fd, peerClosed);
		http::Parser* parser = client->getParser();

		if (!buffer.empty()) parser->append(buffer);
		if (peerClosed) disconnected = true;
		if (disconnected) parser->markEndOfInput();
		if (buffer.empty() && !disconnected) return result;

		while (true) {
			http::Parser::Result parseResult = parser->parse();
			if (parseResult.status == http::Parser::Result::Incomplete) {
				break;
			} else if (parseResult.status == http::Parser::Result::Error) {
				handleParseError(fd, config, parseResult, result);
				client->setState(client::Client::Closing);
				break;
			} else if (parseResult.status == http::Parser::Result::Completed) {
				EventResult processResult = processRequest(fd, config, epollManager, parseResult);
				std::string remainder = parseResult.leftover;

				if (processResult.packet != NULL || !processResult.raw.empty()) {
					result = processResult;
					// If we have a response immediately, we are sending (unless it's CGI which sets
					// Processing) But processRequest sets Processing for CGI. For normal, it
					// returns response. We need to check if processRequest set state to Processing.
					if (client->getState() != client::Client::Processing) {
						client->setState(client::Client::Sending);
					}
				}

				if (processResult.closeFd != -1) {
					result.closeFd = processResult.closeFd;
					client->setState(client::Client::Closing);
				}

				if (!remainder.empty() && client->getState() != client::Client::Processing) {
					// If we have remainder and not waiting for CGI, we might want to process next
					// request? But we are single threaded per client for now? For now, let's just
					// append remainder to parser for next loop if keep-alive
					parser->append(remainder);
					if (parseResult.endOfInput)
						parser->markEndOfInput();
					else
						continue;
				}
				break;
			}
		}
	}

	if (disconnected) {
		result.reset(fd);
		client->setState(client::Client::Closing);
	}

	// If Sending, we might want to register EPOLLOUT?
	// Currently EventResult handles registration via Server.cpp.
	// We just track state here for now.

	return result;
}

void EventHandler::handleParseError(int fd, const config::Config* config,
									http::Parser::Result& parseResult, EventResult& result) {
	const bool hasMessage = !parseResult.errorMessage.empty();
	const std::string& fallbackBody = parseResult.errorMessage;
	const std::string fallbackContentType =
		hasMessage ? http::ContentType::to_string(http::ContentType::CONTENT_TEXT_PLAIN)
				   : std::string();
	http::Packet errorPacket =
		http::response::Factory::createError(parseResult.errorCode, config, fallbackBody,
											 fallbackContentType);
	result.setPacketResponse(fd, errorPacket, false);
}

EventResult EventHandler::processRequest(int fd, const config::Config* config,
										 server::EpollManager& epollManager,
										 http::Parser::Result& parseResult) {
	EventResult result;
	if (!config) {
		http::Packet errorPacket =
			http::response::Factory::createError(http::StatusCode::InternalServerError, config);
		result.setPacketResponse(fd, errorPacket, false);
		return result;
	}

	http::Packet httpRequest = parseResult.packet;
	bool ended = parseResult.endOfInput;
	bool keepAlive = shouldKeepAlive(httpRequest.getHeader().get("Connection"));

	router::RouteDecision decision = _router.route(httpRequest, *config);
	if (decision.action == router::RouteDecision::Cgi) {
		cgi::Executor executor;
		executor.execute(decision, httpRequest, epollManager, _cgiProcessManager, fd);

		client::Client* client = ensureClient(fd, config);
		client->setCgiConfig(decision.server);
		client->setKeepAlive(keepAlive && !ended);
		client->setState(client::Client::Processing);

		if (ended) result.closeFd = fd;
		return result;
	}

	http::Packet httpResponse = _responseBuilder.build(httpRequest, decision, *config);
	result.setPacketResponse(fd, httpResponse, keepAlive && !ended);

	if (ended) result.closeFd = fd;
	return result;
}

void EventHandler::cleanup(int fd, server::EpollManager& epollManager) {
	std::map<int, client::Client*>::iterator it = _clients.find(fd);
	if (it != _clients.end()) {
		delete it->second;
		_clients.erase(it);
	}
	_cgiProcessManager.removeCgiProcess(fd, epollManager);
}

client::Client* EventHandler::ensureClient(int fd, const config::Config* config) {
	std::map<int, client::Client*>::iterator it = _clients.find(fd);
	if (it == _clients.end()) {
		client::Client* client = new client::Client(fd, config);
		_clients.insert(std::make_pair(fd, client));
		return client;
	}
	return it->second;
}

std::string EventHandler::readSocket(int socketFd, bool& peerClosed) const {
	char buffer[server::defaults::BUFFER_SIZE] = {0};
	std::string request;
	peerClosed = false;

	while (true) {
		ssize_t readSize = ::read(socketFd, buffer, server::defaults::BUFFER_SIZE);
		if (readSize > 0) {
			request.append(buffer, readSize);
			if (readSize < server::defaults::BUFFER_SIZE) break;
		} else if (readSize == 0) {
			peerClosed = true;
			break;
		} else {
			if (errno == EAGAIN || errno == EWOULDBLOCK) break;
			return std::string();
		}
	}
	return request;
}
