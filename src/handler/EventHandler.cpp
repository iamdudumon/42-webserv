// EventHandler.cpp
#include "EventHandler.hpp"

#include <unistd.h>

#include <cerrno>

#include "../config/Defaults.hpp"
#include "../handler/utils/response.hpp"
#include "../http/Enums.hpp"
#include "../http/model/Packet.hpp"
#include "../http/serializer/Serializer.hpp"
#include "../server/Defaults.hpp"
#include "../utils/str_utils.hpp"
#include "cgi/Executor.hpp"
#include "cgi/Responder.hpp"

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
	for (std::map<int, http::Parser*>::iterator it = _parsers.begin(); it != _parsers.end(); ++it) {
		delete it->second;
	}
	_parsers.clear();
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
	std::map<int, CgiContext>::iterator ctxIt = _cgiContexts.find(clientFd);
	if (ctxIt != _cgiContexts.end()) {
		if (ctxIt->second.config) config = ctxIt->second.config;
		keepAlive = ctxIt->second.keepAlive;
		_cgiContexts.erase(ctxIt);
	}
	std::string cgiOutput = _cgiProcessManager.getResponse(fd);
	std::string rawResponse =
		(cgiOutput.empty() || !config)
			? http::Serializer::serialize(
				  utils::makeErrorResponse(http::StatusCode::InternalServerError, config),
				  keepAlive)
			: cgi::Responder::makeCgiResponse(cgiOutput, keepAlive);

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
	std::string buffer = readSocket(fd, peerClosed);
	http::Parser* parser = ensureParser(fd, config);

	if (!buffer.empty()) parser->append(buffer);
	if (peerClosed) disconnected = true;
	if (disconnected) parser->markEndOfInput();
	if (buffer.empty() && !disconnected) return result;

	while (true) {
		http::Parser::Result parseResult = parser->parse();
		switch (parseResult.status) {
			case http::Parser::Result::Incomplete:
				break;
			case http::Parser::Result::Error: {
				const bool hasMessage = !parseResult.errorMessage.empty();
				const std::string& fallbackBody = parseResult.errorMessage;
				const std::string fallbackContentType =
					hasMessage ? http::ContentType::to_string(http::ContentType::CONTENT_TEXT_PLAIN)
							   : std::string();
				http::Packet errorPacket =
					utils::makeErrorResponse(parseResult.errorCode, config, fallbackBody,
											 fallbackContentType);
				result.setPacketResponse(fd, errorPacket, false);
				break;
			}
			case http::Parser::Result::Completed: {
				if (!config) {
					http::Packet errorPacket =
						utils::makeErrorResponse(http::StatusCode::InternalServerError, config);
					result.setPacketResponse(fd, errorPacket, false);
					break;
				}

				http::Packet httpRequest = parseResult.packet;
				std::string remainder = parseResult.leftover;
				bool ended = parseResult.endOfInput;
				bool keepAlive = shouldKeepAlive(httpRequest.getHeader().get("Connection"));

				router::RouteDecision decision = _router.route(httpRequest, *config);
				if (decision.action == router::RouteDecision::Cgi) {
					cgi::Executor executor;
					executor.execute(decision, httpRequest, epollManager, _cgiProcessManager, fd);
					_cgiContexts[fd] = CgiContext(decision.server, keepAlive && !ended);

					if (ended) result.closeFd = fd;
					if (!remainder.empty()) {
						parser->append(remainder);
						if (ended) parser->markEndOfInput();
						if (!ended) continue;
					}
					break;
				}

				http::Packet httpResponse =
					_requestHandler.handle(fd, httpRequest, decision, *config);
				result.setPacketResponse(fd, httpResponse, keepAlive && !ended);

				if (!remainder.empty()) {
					parser->append(remainder);
					if (ended) parser->markEndOfInput();
					if (!ended) continue;
				}
				if (ended) result.closeFd = fd;
				break;
			}
		}
		break;
	}

	if (disconnected) result.reset(fd);
	return result;
}

void EventHandler::cleanup(int fd, server::EpollManager& epollManager) {
	std::map<int, http::Parser*>::iterator it = _parsers.find(fd);
	if (it != _parsers.end()) {
		delete it->second;
		_parsers.erase(it);
	}
	_cgiContexts.erase(fd);
	_cgiProcessManager.removeCgiProcess(fd, epollManager);
}

http::Parser* EventHandler::ensureParser(int fd, const config::Config* config) {
	std::map<int, http::Parser*>::iterator it = _parsers.find(fd);
	if (it == _parsers.end()) {
		http::Parser* parser = new http::Parser();
		size_t maxBodySize = static_cast<size_t>(config ? config->getClientMaxBodySize()
														: config::defaults::CLIENT_MAX_BODY_SIZE);
		parser->setMaxBodySize(maxBodySize);
		_parsers.insert(std::make_pair(fd, parser));
		return parser;
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
