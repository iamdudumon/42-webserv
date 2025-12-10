#include "Client.hpp"

using namespace client;

#include <ctime>

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

http::Parser* Client::getParser() {
	return _parser;
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

http::Packet* Client::getRequest() const {
	return _request;
}

void Client::setRequest(http::Packet* request) {
	if (_request) delete _request;
	_request = request;
}

http::Packet* Client::getResponse() const {
	return _response;
}

void Client::setResponse(http::Packet* response) {
	if (_response) delete _response;
	_response = response;
}

void Client::clearRequest() {
	if (_request) {
		delete _request;
		_request = NULL;
	}
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

time_t Client::getLastActivity() const {
	return _lastActivity;
}
