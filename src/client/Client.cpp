#include "Client.hpp"

using namespace client;

Client::Client(int fd, const config::Config* config) :
	_fd(fd), _parser(new http::Parser()), _cgiConfig(NULL), _keepAlive(false), _state(Receiving) {
	if (config) {
		_parser->setMaxBodySize(config->getClientMaxBodySize());
	}
}

Client::~Client() {
	delete _parser;
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
