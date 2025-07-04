#include "../include/Config.hpp"

Config::Config()
{
	_listen = 80;
	_server_name = "localhost";
}

Config&	Config::operator=(const Config& other)
{
	_listen = other._listen;
	_server_name = other._server_name;
	_index = other._index;
	_root = other._root;
	_location = other._location;
	return *this;
}

// Config.hpp 내부에 추가할 getter 함수들
int Config::getListen() const {
    return _listen;
}

const std::string& Config::getServerName() const {
    return _server_name;
}

const std::string& Config::getIndex() const {
    return _index;
}

const std::string& Config::getRoot() const {
    return _root;
}

const std::map<std::string, ConfigLocation>& Config::getLocation() const {
    return _location;
}

// Config.hpp 내부에 추가할 setter 함수들
void Config::setListen(int listen) {
    _listen = listen;
}

void Config::setServerName(const std::string& serverName) {
    _server_name = serverName;
}

void Config::setIndex(const std::string& index) {
    _index = index;
}

void Config::setRoot(const std::string& root) {
    _root = root;
}

void Config::setLocation(const std::string& path, const ConfigLocation& location) {
    _location[path] = location;
}

