// EventHandler.cpp
#include "EventHandler.hpp"

#include <unistd.h>

#include <cerrno>
#include <ctime>

#include "../client/Defaults.hpp"
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
	client->updateLastActivity();

	if (client->getState() == client::Client::Receiving) {
		std::string buffer = readSocket(fd, peerClosed);
		http::Parser* parser = client->getParser();

		if (buffer.empty()) parser->append(buffer);
		if (peerClosed) disconnected = true;
		if (disconnected) parser->markEndOfInput();
		if (buffer.empty() && !disconnected && !parser->hasUnconsumedInput()) return result;

		while (true) {
			http::Parser::Result parseResult = parser->parse();
			if (parseResult.status == http::Parser::Result::Incomplete) {
				epollManager.modify(fd, EPOLLIN | EPOLLRDHUP);
				break;
			} else if (parseResult.status == http::Parser::Result::Error) {
				handleParseError(client, config, parseResult);
				break;
			} else if (parseResult.status == http::Parser::Result::Completed) {
				client->setRequest(parseResult.packet);
				processRequest(client, config, epollManager);

				std::string remainder = parseResult.leftover;
				if (!remainder.empty()) {
					parser->append(remainder);
					if (parseResult.endOfInput) parser->markEndOfInput();
				}

				if (client->getState() == client::Client::Processing) break;
				if (!remainder.empty()) continue;
				break;
			}
		}
	}

	if (disconnected) client->setState(client::Client::Closing);
	if (client->getState() == client::Client::Sending && client->getResponse()) {
		result.setPacketResponse(fd, *client->getResponse(), client->isKeepAlive());
		if (client->isKeepAlive() && !disconnected) {
			client->clearRequest();
			client->clearResponse();
			client->setState(client::Client::Receiving);

			if (client->getParser()->hasUnconsumedInput()) {
				epollManager.modify(fd, EPOLLIN | EPOLLOUT);
			}
		} else {
			client->setState(client::Client::Closing);
		}
	} else if (client->getState() == client::Client::Closing) {
		if (client->getResponse())
			result.setPacketResponse(fd, *client->getResponse(), false);
		else
			result.reset(fd);
	}
	return result;
}

void EventHandler::handleParseError(client::Client* client, const config::Config* config,
									http::Parser::Result& parseResult) {
	const bool hasMessage = !parseResult.errorMessage.empty();
	const std::string& fallbackBody = parseResult.errorMessage;
	const std::string fallbackContentType =
		hasMessage ? http::ContentType::to_string(http::ContentType::CONTENT_TEXT_PLAIN)
				   : std::string();
	http::Packet errorPacket =
		http::response::Factory::createError(parseResult.errorCode, config, fallbackBody,
											 fallbackContentType);

	client->setResponse(new http::Packet(errorPacket));
	client->setState(client::Client::Closing);
}

void EventHandler::processRequest(client::Client* client, const config::Config* config,
								  server::EpollManager& epollManager) {
	http::Packet* httpRequest = client->getRequest();
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
	time_t now = std::time(NULL);
	std::map<int, client::Client*>::iterator it = _clients.begin();

	while (it != _clients.end()) {
		if (now - it->second->getLastActivity() > client::defaults::CONNECTION_TIMEOUT) {
			int fd = it->first;
			std::map<int, client::Client*>::iterator next = it;
			++next;

			cleanup(fd, epollManager);
			epollManager.remove(fd);

			it = next;
		} else {
			++it;
		}
	}
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
