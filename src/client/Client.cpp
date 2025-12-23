#include "Client.hpp"

#include "exception/Exception.hpp"

using namespace client;

#include <unistd.h>

#include <cerrno>
#include <ctime>

#include "../server/Defaults.hpp"
#include "Defaults.hpp"

Client::Client(int fd, const config::Config* config) :
	_fd(fd),
	_parser(new http::Parser()),
	_cgiConfig(NULL),
	_keepAlive(false),
	_lastActivity(std::time(NULL)),
	_state(Receiving),
	_request(NULL),
	_response(NULL) {
	if (config) {
		_parser->setMaxBodySize(config->getClientMaxBodySize());
	}
}

Client::~Client() {
	delete _parser;
	if (_request) delete _request;
	if (_response) delete _response;
}

int Client::getFd() const {
	return _fd;
}

const config::Config* Client::getCgiConfig() const {
	return _cgiConfig;
}

void Client::setCgiConfig(const config::Config* config) {
	_cgiConfig = config;
}

bool Client::isKeepAlive() const {
	return _keepAlive;
}

void Client::setKeepAlive(bool keepAlive) {
	_keepAlive = keepAlive;
}

Client::State Client::getState() const {
	return _state;
}

void Client::setState(State state) {
	_state = state;
}

http::Packet* Client::getResponse() const {
	return _response;
}

void Client::setResponse(http::Packet* response) {
	if (_response) delete _response;
	_response = response;
}

void Client::clearResponse() {
	if (_response) {
		delete _response;
		_response = NULL;
	}
}

void Client::updateLastActivity() {
	_lastActivity = std::time(NULL);
}

bool Client::isTimedOut(time_t now) const {
	return (now - _lastActivity) > defaults::CONNECTION_TIMEOUT;
}

void Client::processData(const std::string& data) {
	_parser->append(data);

	while (true) {
		if (_request) break;

		http::Parser::Result result = _parser->parse();

		if (result.status == http::Parser::Result::Completed) {
			_request = result.packet;
			if (!result.leftover.empty()) {
				_parser->append(result.leftover);
			}
			break;
		} else if (result.status == http::Parser::Result::Error)
			throw Exception(result.errorCode, result.errorMessage);
		else
			break;
	}
}

bool Client::hasRequest() const {
	return _request != NULL;
}

http::Packet* Client::takeRequest() {
	http::Packet* packet = _request;
	_request = NULL;
	return packet;
}

bool Client::hasUnconsumedInput() const {
	return _parser->hasUnconsumedInput();
}

void Client::markEndOfInput() {
	_parser->markEndOfInput();
}

std::string Client::readSocket(bool& peerClosed) const {
	char buffer[server::defaults::BUFFER_SIZE] = {0};
	std::string request;
	peerClosed = false;

	while (true) {
		ssize_t readSize = ::read(_fd, buffer, server::defaults::BUFFER_SIZE);
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
