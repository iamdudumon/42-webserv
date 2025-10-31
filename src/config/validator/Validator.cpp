// Validator.cpp
#include "Validator.hpp"

#include "../../utils/str_utils.hpp"
#include "../exception/Exception.hpp"

using namespace config;

void Validator::validatePort(const Config& config, std::set<int>& ports) {
	int port = config.getListen();
	if (port < 0) {
		throw Exception("[emerg] no \"listen\" directive in server block");
	}
	std::pair<std::set<int>::iterator, bool> inserted = ports.insert(port);
	if (!inserted.second) {
		throw Exception("[emerg] duplicate listen on " + int_tostr(port));
	}
}

void Validator::validateLocations(const Config& config) {
	const std::map<std::string, LocationConfig>& locations = config.getLocation();
	if (locations.empty()) {
		throw Exception("[emerg] no \"location\" block defined");
	}

	for (std::map<std::string, LocationConfig>::const_iterator it = locations.begin();
		 it != locations.end(); ++it) {
		validateLocation(it->first, it->second);
	}
}

void Validator::validateLocation(const std::string& path, const LocationConfig& loc) {
	if (loc._root.empty()) {
		throw Exception("[emerg] no \"root\" in location \"" + path + "\"");
	}
}

void Validator::validate(const std::map<int, Config>& configs) {
	std::set<int> ports;
	for (std::map<int, Config>::const_iterator it = configs.begin(); it != configs.end(); ++it) {
		validatePort(it->second, ports);
		validateLocations(it->second);
	}
}
