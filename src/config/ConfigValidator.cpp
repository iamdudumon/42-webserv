#include "ConfigValidator.hpp"

void ConfigValidator::validate(const std::vector<Config>& configs) {
	std::set<int> ports;
	for (unsigned long i = 0; i < configs.size(); i++) {
		int port = configs[i].getListen();
		if (port < 0) {
			throw ConfigException("[emerg] no \"listen\" directive in server block");
		}
		std::pair<std::set<int>::iterator, bool> result = ports.insert(port);
		if (!result.second) {
			throw ConfigException("[emerg] duplicate listen on " + int_tostr(port));
		}

		const std::map<std::string, ConfigLocation>& locations = configs[i].getLocation();
		if (locations.empty()) {
			throw ConfigException("[emerg] no \"location\" block defined");
		}

		for (std::map<std::string, ConfigLocation>::const_iterator it = locations.begin();
			 it != locations.end(); ++it) {
			const std::string& path = it->first;
			const ConfigLocation& loc = it->second;

			if (loc._root.empty()) {
				throw ConfigException("[emerg] no \"root\" in location \"" + path + "\"");
			}
		}
	}
}
