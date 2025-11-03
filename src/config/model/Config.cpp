// Config.cpp
#include "Config.hpp"

#include "../Defaults.hpp"
#include "../exception/Exception.hpp"

using namespace config;

Config::Config() :
	_server_name(defaults::SERVER_NAME()),
	_listen(-1),
	_auto_index(false),
	_client_max_body_size(defaults::CLIENT_MAX_BODY_SIZE),
	_index() {}

Config::Config(const Config& other) {
	*this = other;
}

Config& Config::operator=(const Config& other) {
	_server_name = other._server_name;
	_listen = other._listen;
	_auto_index = other._auto_index;
	_client_max_body_size = other._client_max_body_size;
	_upload_path = other._upload_path;
	_index = other._index;
	_root = other._root;
	_location = other._location;
	_error_pages = other._error_pages;
	return *this;
}

bool Config::getAutoIndex() const {
	return _auto_index;
}

int Config::getListen() const {
	return _listen;
}

long long Config::getClientMaxBodySize() const {
	return _client_max_body_size;
}

const std::string& Config::getServerName() const {
	return _server_name;
}

const std::string& Config::getUploadPath() const {
	return _upload_path;
}

const std::string& Config::getIndex() const {
	return _index;
}

const std::string& Config::getRoot() const {
	return _root;
}

const std::map<int, std::string>& Config::getErrorPages() const {
	return _error_pages;
}

const std::map<std::string, LocationConfig>& Config::getLocation() const {
	return _location;
}

void Config::setAutoIndex(bool auto_index) {
	_auto_index = auto_index;
}

void Config::setListen(int listen) {
	_listen = listen;
}

void Config::setClientMaxBodySize(long long client_max_body_size) {
	_client_max_body_size = client_max_body_size;
}

void Config::setServerName(const std::string& serverName) {
	_server_name = serverName;
}

void Config::setUploadPath(const std::string& upload_path) {
	_upload_path = upload_path;
}

void Config::setIndex(const std::string& index) {
	_index = index;
}

void Config::setRoot(const std::string& root) {
	_root = root;
}

void Config::setErrorPage(int statusCode, const std::string& path) {
	_error_pages[statusCode] = path;
}

void Config::initLocation(const std::string& path) {
	if (_location.find(path) != _location.end())
		throw Exception("[emerg] Invalid configuration: duplicate location");
	LocationConfig location;
	location._root = _root;
	location._index = _index;
	_location[path] = location;
}

void Config::setLocationRoot(const std::string& path, const std::string& root) {
	_location[path]._root = root;
}

void Config::setLocationIndex(const std::string& path, const std::string& index) {
	_location[path]._index = index;
}

void Config::setLocationAllowMethods(const std::string& path,
									 const std::vector<std::string>& methods) {
	_location[path]._allow_methods = methods;
}

const std::string& Config::getLocationRoot(const std::string& path) const {
	std::map<std::string, LocationConfig>::const_iterator it = _location.find(path);
	if (it == _location.end()) {
		throw Exception("Invalid path: " + path);
	}
	return it->second._root;
}

const std::string& Config::getLocationIndex(const std::string& path) const {
	std::map<std::string, LocationConfig>::const_iterator it = _location.find(path);
	if (it == _location.end()) {
		throw Exception("Invalid path: " + path);
	}
	return it->second._index;
}

const std::vector<std::string>& Config::getLocationAllowMethods(const std::string& path) const {
	std::map<std::string, LocationConfig>::const_iterator it = _location.find(path);
	if (it == _location.end()) {
		throw Exception("Invalid path: " + path);
	}
	return it->second._allow_methods;
}
