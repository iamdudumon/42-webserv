#include "ConfigValidator.hpp"

void ConfigValidator::validatePort(const Config& config, std::set<int>& ports) {
	int port = config.getListen();
	if (port < 0) {
		throw ConfigException("[emerg] no \"listen\" directive in server block");
	}
	std::pair<std::set<int>::iterator, bool> result = ports.insert(port);
	if (!result.second) {
		throw ConfigException("[emerg] duplicate listen on " + int_tostr(port));
	}
}

void ConfigValidator::validateLocations(const Config& config) {
	const std::map<std::string, ConfigLocation>& locations = config.getLocation();
	if (locations.empty()) {
		throw ConfigException("[emerg] no \"location\" block defined");
	}

	for (std::map<std::string, ConfigLocation>::const_iterator it = locations.begin();
		 it != locations.end(); ++it) {
		validateLocation(it->first, it->second);
	}
}

void ConfigValidator::validateLocation(const std::string& path, const ConfigLocation& loc) {
	if (loc._root.empty()) {
		throw ConfigException("[emerg] no \"root\" in location \"" + path + "\"");
	}
}

void ConfigValidator::validate(const std::vector<Config>& configs) {
	std::set<int> ports;
	for (unsigned long i = 0; i < configs.size(); ++i) {
		validatePort(configs[i], ports);
		validateLocations(configs[i]);
	}
}
